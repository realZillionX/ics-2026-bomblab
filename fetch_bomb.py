"""下载并解包自己的 bomb，也支持 --file 导入已下载的附件。"""

import argparse
import hashlib
import http.client
import io
import json
from pathlib import Path
import re
import sys
import tarfile
import time
import urllib.error
import urllib.parse
import urllib.request

RELEASE_BASE = "https://github.com/ICS-26Fall-FDU/BombLab/releases/download/bombs-2026fall-v1.2"
MEMBERS = ("bomb", "package.json")
ROOT = Path(__file__).resolve().parent
DOWNLOAD_TIMEOUT = 15
DOWNLOAD_ATTEMPTS = 3
NETWORK_ERRORS = (urllib.error.URLError, OSError, http.client.HTTPException)


class DownloadError(Exception):
    pass


def fail(message):
    print(f"错误：{message}", file=sys.stderr)
    raise SystemExit(1)


def read_student_id():
    values = {}
    for raw in (ROOT / "config.txt").read_text(encoding="utf-8-sig").splitlines():
        line = raw.split("#", 1)[0].strip()
        if "=" in line:
            key, value = (part.strip() for part in line.split("=", 1))
            values[key] = value
    student_id = values.get("student_id", "")
    if not re.fullmatch(r"[A-Za-z0-9]{11}", student_id):
        fail("请先在 config.txt 中填写你自己的 11 位学号（数字或英文字母），"
             "替换 student_id = FILL_IN_STUDENT_ID 中的占位符。")
    return student_id


def download_id(student_id):
    return hashlib.sha256(f"bomblab-download-v1:{student_id}".encode("ascii")).hexdigest()


def missing(error):
    return (isinstance(error, urllib.error.HTTPError) and error.code == 404
            or isinstance(getattr(error, "reason", None), FileNotFoundError))


def retryable(error):
    if isinstance(error, urllib.error.HTTPError):
        return error.code in (408, 429) or 500 <= error.code < 600
    return not missing(error)


def error_message(error):
    if isinstance(error, urllib.error.HTTPError):
        if error.code == 403 and error.headers.get("X-RateLimit-Remaining") == "0":
            return "GitHub API 请求次数达到上限，请稍后重试"
        return f"HTTP {error.code}"
    reason = getattr(error, "reason", error)
    if isinstance(reason, TimeoutError):
        return "连接或读取超时"
    if isinstance(reason, http.client.IncompleteRead):
        return "下载中断，数据不完整"
    return f"网络连接失败（{type(reason).__name__}）"


def read_url(url, label, accept="application/octet-stream", attempts=DOWNLOAD_ATTEMPTS):
    for attempt in range(1, attempts + 1):
        print(f"{label}（第 {attempt}/{attempts} 次）", flush=True)
        request = urllib.request.Request(url, headers={
            "User-Agent": "BombLab-fetch", "Accept": accept,
        })
        try:
            with urllib.request.urlopen(request, timeout=DOWNLOAD_TIMEOUT) as response:
                return response.read()
        except NETWORK_ERRORS as error:
            if attempt == attempts or not retryable(error):
                raise
            print(f"{error_message(error)}，稍后重试。", file=sys.stderr, flush=True)
            time.sleep(attempt)


class Downloader:
    def __init__(self):
        parsed = urllib.parse.urlsplit(RELEASE_BASE)
        match = re.fullmatch(r"/([^/]+)/([^/]+)/releases/download/(.+)", parsed.path)
        self.api_base = None
        self.release_api = None
        self.assets = None
        self.prefer_api = False
        if parsed.scheme == "https" and parsed.hostname == "github.com" and match:
            owner, repository, tag = (urllib.parse.quote(urllib.parse.unquote(part), safe="")
                                      for part in match.groups())
            self.api_base = f"https://api.github.com/repos/{owner}/{repository}"
            self.release_api = f"{self.api_base}/releases/tags/{tag}"

    def api_download(self, name):
        if self.assets is None:
            data = read_url(self.release_api, "官方 API：查询 Release 附件", "application/vnd.github+json")
            try:
                release = json.loads(data)
                if release.get("draft") or not isinstance(release.get("assets"), list):
                    raise ValueError
                self.assets = {asset["name"]: asset for asset in release["assets"]}
            except (ValueError, KeyError, TypeError, AttributeError) as error:
                raise DownloadError("GitHub API 返回的 Release 信息无效") from error
        asset = self.assets.get(name)
        if asset is None:
            return None
        asset_id = asset.get("id")
        if not isinstance(asset_id, int) or asset_id <= 0:
            raise DownloadError("GitHub API 返回的附件编号无效")
        data = read_url(f"{self.api_base}/releases/assets/{asset_id}", f"官方 API：下载 {name}")
        digest = asset.get("digest")
        if isinstance(digest, str) and digest.startswith("sha256:"):
            if digest != "sha256:" + hashlib.sha256(data).hexdigest():
                raise DownloadError(f"{name} 与 GitHub 公布的 SHA-256 不符，请重新下载")
        return data

    def download(self, name):
        if "@" in RELEASE_BASE:
            fail("这份 fetch_bomb.py 还没有配置下载地址，请从课程模板仓库获取")
        url = f"{RELEASE_BASE}/{name}"
        routes = [True, False] if self.prefer_api else [False, True]
        if self.api_base is None:
            routes = [False]
        errors = []
        normal_error = None
        for use_api in routes:
            label = "官方 API" if use_api else "普通入口"
            try:
                if use_api:
                    data = self.api_download(name)
                else:
                    attempts = DOWNLOAD_ATTEMPTS if self.prefer_api or not self.api_base else 1
                    data = read_url(url, f"普通入口：下载 {name}", attempts=attempts)
                self.prefer_api = use_api
                return data
            except DownloadError as error:
                fail(str(error))
            except NETWORK_ERRORS as error:
                if not use_api:
                    normal_error = error
                    if missing(error) and not self.api_base:
                        return None
                reason = error_message(error)
                errors.append(f"{label}：{reason}")
                print(f"{label}失败：{reason}。", file=sys.stderr, flush=True)
        if self.api_base and not self.prefer_api and retryable(normal_error):
            try:
                return read_url(url, f"重试普通入口：下载 {name}", attempts=DOWNLOAD_ATTEMPTS - 1)
            except NETWORK_ERRORS as error:
                errors.append(f"重试普通入口：{error_message(error)}")
        fail(f"无法下载 {name}。{'；'.join(errors)}。\n"
             "请确认课程仓库和 Release 已公开，并检查网络或代理设置。\n"
             f"也可用浏览器下载附件，再运行 python3 fetch_bomb.py --file {name}")


def main():
    parser = argparse.ArgumentParser(description="下载并解包个人 bomb")
    selected = parser.add_mutually_exclusive_group()
    selected.add_argument("--file", type=Path, help="已经手动下载的个人炸弹压缩包")
    selected.add_argument("--print-url", action="store_true", help="显示自己的附件链接，供浏览器下载")
    args = parser.parse_args()
    student_id = read_student_id()
    identifier = download_id(student_id)
    name = f"bomb-{identifier}.tar.gz"

    if args.print_url:
        if "@" in RELEASE_BASE:
            fail("这份 fetch_bomb.py 还没有配置下载地址，请从课程模板仓库获取")
        print(f"{RELEASE_BASE}/{name}")
        return

    if args.file:
        data, sums = args.file.read_bytes(), None
    else:
        downloader = Downloader()
        data = downloader.download(name)
        if data is None:
            fail(f"没有找到学号 {student_id} 的 bomb。请检查 config.txt 中的学号；"
                 "确认无误仍然失败时请联系助教。")
        sums = downloader.download("SHA256SUMS")
        if sums is None:
            fail("Release 缺少 SHA256SUMS，无法校验炸弹，请联系助教。")
    if sums is not None:
        expected = {line.split()[1]: line.split()[0]
                    for line in sums.decode().splitlines() if len(line.split()) == 2}
        if expected.get(name) != hashlib.sha256(data).hexdigest():
            fail(f"{name} 的校验和不符，可能下载不完整，请重新下载")

    with tarfile.open(fileobj=io.BytesIO(data), mode="r:gz") as archive:
        contents = {}
        for member in MEMBERS:
            info = archive.getmember(member)
            if not info.isfile():
                fail(f"压缩包中的 {member} 不是普通文件")
            contents[member] = archive.extractfile(info).read()
    package = json.loads(contents["package.json"])
    if not isinstance(package, dict):
        fail("压缩包中的 package.json 无效")
    legacy_import = args.file is not None and package.get("student_id") == student_id
    if package.get("download_id") != identifier and not legacy_import:
        fail("压缩包与 config.txt 中的学号不一致，请下载自己的附件")
    files = package.get("files")
    if not isinstance(files, dict) or files.get("bomb") != hashlib.sha256(contents["bomb"]).hexdigest():
        fail("压缩包中的 bomb 校验和不符，请重新下载")

    for member, payload in contents.items():
        target = ROOT / member
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(payload)
    (ROOT / "bomb").chmod(0o755)
    print(f"已领取个人 bomb（package {package['package_id']}）。")
    print("下一步：make；tutor 教学在 tutor/，可运行 make test-tutor")


if __name__ == "__main__":
    main()
