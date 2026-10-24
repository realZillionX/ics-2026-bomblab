"""Public self-checks: the six phases and the required Honor phase."""

import argparse
import json
from pathlib import Path

from runner import Result, answer_line, run_limited


def phase_transcript(result):
    if result.timed_out:
        return None, "timeout"
    if result.returncode:
        return None, "solver_error"
    if result.truncated:
        return None, "output_limit"
    lines = result.stdout.splitlines(keepends=True)
    if len(lines) != 6:
        return None, "format_error"
    for line in lines:
        _, error = answer_line(Result(0, line, b""))
        if error:
            return None, error
    return result.stdout, None


def check_seed(root, seed):
    flags = ["--seed", str(seed)]
    solver = str(root / "defuser")
    bomb = str(root / "bomb")
    result = run_limited([solver, *flags], cwd=root)
    prefix, error = phase_transcript(result)
    if error:
        return "prerequisite_" + error
    checked = run_limited([bomb, *flags], cwd=root, data=prefix)
    if checked.timed_out:
        return "prerequisite_timeout"
    if checked.truncated:
        return "prerequisite_output_limit"
    if checked.returncode:
        return "prerequisite_wrong_answer"
    result = run_limited([solver, "--secret", *flags], cwd=root)
    answer, error = answer_line(result)
    if error:
        return error
    checked = run_limited([bomb, *flags], cwd=root, data=prefix + answer)
    if checked.timed_out:
        return "timeout"
    if checked.truncated:
        return "output_limit"
    if checked.returncode:
        return "wrong_answer"
    if b"Secret phase defused." not in checked.stdout.splitlines():
        return "secret_not_reached"
    return None


def check_secret(root, seeds):
    """Report every Honor seed; unfinished solutions do not pass."""
    errors = [check_seed(root, seed) for seed in seeds]
    for seed, error in zip(seeds, errors):
        print(f"Secret seed {seed}: {'PASS' if error is None else 'FAIL (' + error + ')'}")
    passed = errors.count(None)
    print(f"Secret public tests: {passed}/{len(seeds)}")
    return passed == len(seeds)


def main():
    parser = argparse.ArgumentParser(description="Run reproducible public checks")
    selected = parser.add_mutually_exclusive_group()
    selected.add_argument("--phase", type=int, choices=range(7))
    selected.add_argument("--secret", action="store_true", help="check only the Honor phase")
    args = parser.parse_args()
    root = Path(__file__).resolve().parent
    if args.secret:
        for name in ("bomb", "defuser"):
            if not (root / name).is_file():
                print(f"Missing {name}: download your personal bomb and run make first.")
                return 1
    cases = json.loads((root / "tests/public.json").read_text())
    if args.secret:
        return 0 if check_secret(root, cases["seeds"]) else 1
    phases = [args.phase] if args.phase is not None else cases["phases"]
    if args.phase == 0:
        root = root / "tutor"
    total = passed = 0
    for phase in phases:
        for seed in cases["seeds"]:
            flags = ["--seed", str(seed), "--phase", str(phase)]
            result = run_limited([str(root / "defuser"), *flags], cwd=root)
            answer, error = answer_line(result)
            if answer is not None:
                checked = run_limited([str(root / "bomb"), *flags], cwd=root, data=answer)
                if checked.returncode or checked.timed_out:
                    error = "wrong_answer"
            ok = error is None
            total += 1
            passed += ok
            print(f"Phase {phase} seed {seed}: {'PASS' if ok else 'FAIL (' + error + ')'}")
    print(f"Public tests: {passed}/{total}")
    if args.phase is not None:
        return 0 if passed == total else 1
    if passed != total:
        print("Secret phase: skipped, the six phases must pass first")
        return 1
    return 0 if check_secret(root, cases["seeds"]) else 1


if __name__ == "__main__":
    raise SystemExit(main())
