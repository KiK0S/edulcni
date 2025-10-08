#!/usr/bin/env python3
"""Build a consolidated portfolio directory for static examples.

This script assumes all static examples were built already ("make examples-static").
It will run each compiled example, collect the exported visualization assets, move
those assets into a common portfolio directory, and generate an index page with
convenient redirects.
"""
from __future__ import annotations

import html
import json
import os
import re
import shutil
import subprocess
from dataclasses import dataclass
from pathlib import Path
from typing import List

REPO_ROOT = Path(__file__).resolve().parents[1]
STATIC_EXAMPLES_DIR = REPO_ROOT / "examples" / "static"
BUILD_OUTPUT_DIR = REPO_ROOT / "out"
PORTFOLIO_DIR = REPO_ROOT / "portfolio"

INITIALIZE_PATTERN = re.compile(r"initialize\s*\(\s*\"([^\"]+)\"")


@dataclass
class ExampleMetadata:
    name: str
    output_path: Path
    display_name: str
    summary: str | None


def extract_output_path(example_source: Path) -> tuple[Path, str | None]:
    """Return the output directory and optional summary comment for an example."""
    text = example_source.read_text(encoding="utf-8", errors="ignore")
    match = INITIALIZE_PATTERN.search(text)
    if not match:
        raise ValueError(f"Could not find edulcni::initialize path in {example_source}")

    raw_path = match.group(1)
    comment_summary = None

    # Attempt to capture a leading comment block as the summary.
    comment_match = re.search(r"/\*\*(.*?)\*/", text, re.DOTALL)
    if comment_match:
        summary = comment_match.group(1).strip()
        # Use the first non-empty line as the summary sentence.
        for line in summary.splitlines():
            stripped = line.strip(" *")
            if stripped:
                comment_summary = stripped
                break

    return Path(raw_path), comment_summary


def derive_display_name(example_name: str) -> str:
    words = example_name.replace("_", " ").split()
    return " ".join(w.capitalize() for w in words)


def move_output(source_path: Path, destination_dir: Path) -> None:
    if destination_dir.exists():
        shutil.rmtree(destination_dir)
    destination_dir.parent.mkdir(parents=True, exist_ok=True)
    shutil.move(str(source_path), str(destination_dir))


def make_redirect_page(example: ExampleMetadata) -> None:
    redirect_target = f"{example.name}/index.html"
    redirect_html = f"""<!DOCTYPE html>
<html lang=\"en\">
  <head>
    <meta charset=\"utf-8\">
    <title>{html.escape(example.display_name)}</title>
    <meta http-equiv=\"refresh\" content=\"0; url={redirect_target}\">
    <link rel=\"canonical\" href=\"{redirect_target}\">
    <style>
      body {{
        font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
        padding: 3rem 1.5rem;
        background: #111827;
        color: #f9fafb;
        text-align: center;
      }}
      a {{ color: #38bdf8; }}
    </style>
  </head>
  <body>
    <h1>Redirecting to {html.escape(example.display_name)}</h1>
    <p>If you are not redirected automatically, <a href=\"{redirect_target}\">click here to open the visualization.</a></p>
  </body>
</html>
"""
    (PORTFOLIO_DIR / f"{example.name}.html").write_text(redirect_html, encoding="utf-8")


def make_index_page(examples: List[ExampleMetadata]) -> None:
    list_items = []
    for example in examples:
        summary_text = html.escape(example.summary) if example.summary else ""
        description = f"<p class=\"summary\">{summary_text}</p>" if summary_text else ""
        list_items.append(
            f"""<li><a href=\"{example.name}.html\">{html.escape(example.display_name)}</a>{description}</li>"""
        )

    index_html = f"""<!DOCTYPE html>
<html lang=\"en\">
  <head>
    <meta charset=\"utf-8\">
    <title>Edulcni Static Example Portfolio</title>
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">
    <style>
      :root {{
        color-scheme: dark;
      }}
      body {{
        margin: 0;
        font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
        background: #0b1120;
        color: #e2e8f0;
        min-height: 100vh;
        display: flex;
        justify-content: center;
      }}
      main {{
        width: min(900px, 100%);
        padding: clamp(1.5rem, 5vw, 4rem);
      }}
      h1 {{
        font-size: clamp(2rem, 5vw, 3.25rem);
        margin-bottom: 1.25rem;
        text-align: center;
      }}
      p.lead {{
        text-align: center;
        color: #94a3b8;
        margin-bottom: 2rem;
      }}
      ul.examples {{
        list-style: none;
        padding: 0;
        margin: 0;
        display: grid;
        gap: 1.25rem;
      }}
      ul.examples li {{
        background: rgba(30, 41, 59, 0.75);
        border: 1px solid rgba(148, 163, 184, 0.2);
        border-radius: 1rem;
        padding: 1.25rem;
        transition: transform 0.2s ease, border-color 0.2s ease;
      }}
      ul.examples li:hover {{
        transform: translateY(-4px);
        border-color: rgba(56, 189, 248, 0.6);
      }}
      ul.examples a {{
        color: #38bdf8;
        font-weight: 600;
        font-size: 1.1rem;
        text-decoration: none;
      }}
      ul.examples p.summary {{
        margin: 0.65rem 0 0;
        color: #cbd5f5;
        line-height: 1.4;
      }}
      footer {{
        margin-top: 2.5rem;
        text-align: center;
        color: #64748b;
        font-size: 0.9rem;
      }}
    </style>
  </head>
  <body>
    <main>
      <h1>Edulcni Static Examples</h1>
      <p class=\"lead\">Explore the growing gallery of algorithm and data-structure visualizations generated from the static examples.</p>
      <ul class=\"examples\">
        {''.join(list_items)}
      </ul>
      <footer>
        Built automatically from the <code>examples/static</code> suite.
      </footer>
    </main>
  </body>
</html>
"""
    (PORTFOLIO_DIR / "index.html").write_text(index_html, encoding="utf-8")


def ensure_portfolio_dir() -> None:
    if PORTFOLIO_DIR.exists():
        shutil.rmtree(PORTFOLIO_DIR)
    PORTFOLIO_DIR.mkdir(parents=True, exist_ok=True)


def main() -> None:
    ensure_portfolio_dir()

    examples: List[ExampleMetadata] = []

    for example_source in sorted(STATIC_EXAMPLES_DIR.glob("*.cpp")):
        example_name = example_source.stem
        binary_path = BUILD_OUTPUT_DIR / example_name
        if not binary_path.exists():
            raise FileNotFoundError(
                f"Compiled binary {binary_path} missing. Run 'make examples-static' first."
            )

        output_spec, summary = extract_output_path(example_source)
        temp_binary = binary_path.with_name(f"{binary_path.name}_runner")
        if temp_binary.exists():
            temp_binary.unlink()
        binary_path.rename(temp_binary)

        try:
            subprocess.run([str(temp_binary)], cwd=REPO_ROOT, check=True)

            output_path = output_spec
            if not output_path.is_absolute():
                output_path = (REPO_ROOT / output_path).resolve()

            if not output_path.exists():
                raise FileNotFoundError(
                    f"Expected visualization output at {output_path} after running {temp_binary}"
                )

            destination_dir = PORTFOLIO_DIR / example_name
            move_output(output_path, destination_dir)

            metadata = ExampleMetadata(
                name=example_name,
                output_path=destination_dir,
                display_name=derive_display_name(example_name),
                summary=summary,
            )
            make_redirect_page(metadata)
            examples.append(metadata)
        finally:
            try:
                if temp_binary.exists():
                    temp_binary.rename(binary_path)
            except OSError:
                # Best-effort cleanup; leave the renamed binary in place if rename fails.
                pass

    make_index_page(examples)

    # Persist metadata to make debugging easier.
    metadata_file = PORTFOLIO_DIR / "portfolio.json"
    metadata_payload = [
        {
            "name": example.name,
            "display_name": example.display_name,
            "output_path": os.path.relpath(example.output_path, PORTFOLIO_DIR),
            "summary": example.summary,
        }
        for example in examples
    ]
    metadata_file.write_text(
        json.dumps(metadata_payload, indent=2, ensure_ascii=False),
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
