"""Resource bounds for local tests. This is NOT a security sandbox."""

from dataclasses import dataclass
import os
import resource
import signal
import subprocess
import tempfile


@dataclass
class Result:
    returncode: int
    stdout: bytes
    stderr: bytes
    timed_out: bool = False
    truncated: bool = False


def run_limited(command, *, cwd=None, data=b"", timeout=3, compile_job=False):
    def limits():
        resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
        resource.setrlimit(resource.RLIMIT_CPU, (30, 31) if compile_job else (2, 3))
        resource.setrlimit(resource.RLIMIT_AS, (512 << 20, 512 << 20))
        resource.setrlimit(resource.RLIMIT_FSIZE, (8 << 20, 8 << 20) if compile_job else (65536, 65536))

    with tempfile.TemporaryFile() as output, tempfile.TemporaryFile() as errors:
        process = subprocess.Popen(command, cwd=cwd, stdin=subprocess.PIPE,
                                   stdout=output, stderr=errors, start_new_session=True,
                                   preexec_fn=limits, env={"PATH": os.defpath, "LC_ALL": "C"})
        timed_out = False
        try:
            process.communicate(data, timeout=timeout)
        except subprocess.TimeoutExpired:
            timed_out = True
        finally:
            try:
                os.killpg(process.pid, signal.SIGKILL)
            except ProcessLookupError:
                pass
            process.communicate()
        output.seek(0)
        errors.seek(0)
        stdout, stderr = output.read(65537), errors.read(65537)
        return Result(process.returncode, stdout[:65536], stderr[:65536], timed_out,
                      len(stdout) > 65536 or len(stderr) > 65536)


def answer_line(result):
    if result.timed_out:
        return None, "timeout"
    if result.returncode:
        return None, "solver_error"
    if result.truncated or len(result.stdout) >= 256:
        return None, "output_limit"
    try:
        text = result.stdout.decode("ascii")
    except UnicodeDecodeError:
        return None, "format_error"
    if not text.endswith("\n") or "\n" in text[:-1] or "\r" in text or "\x00" in text or not text[:-1]:
        return None, "format_error"
    return result.stdout, None
