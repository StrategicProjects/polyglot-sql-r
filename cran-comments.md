## Resubmission

This is a resubmission of 0.1.2. The first submission (2026-09-30) passed the
incoming pre-tests but the linux-arm64 re-check reported

    checking compiled code ... NOTE
    Note: information on .o files is not available
    File 'polyglotSQL/libs/polyglotSQL.so': Found 'abort', possibly from 'abort' (C)

and Uwe Ligges asked for that reference to be removed.

The reference did not come from this package's code or from the bundled
crates, but from the Rust standard library, which calls `abort()` as a last
resort (a failed memory allocation, a panic raised while another panic is
being handled, a panic reaching a frame that cannot unwind); ordinary panics
unwind and are converted into R errors at the FFI boundary. In this
resubmission those last-resort calls are redirected at link time to a function
in `src/entrypoint.c` that raises an R error (GNU ld `--wrap=abort` on Linux,
`-alias` on macOS, see `tools/config.R`). The linked shared object no longer
contains an `abort` symbol, defined or undefined, on either platform, and a
unit test checks this with `nm -Pg`. Verified with `tools:::check_so_symbols()`
on the installed library on linux-arm64 (Docker, rocker/r-ver) and macOS.
Windows is unchanged; the Windows check did not report the symbol.

Everything else is as in the first submission; the "Days since last update"
NOTE is because 0.1.2 fixes the check failures of 0.1.1 listed below.

## Submission

polyglotSQL 0.1.2 is a patch release that fixes the check failures of 0.1.1
caused by the cost of compiling the embedded Rust engine:

* **ERROR on r-release-macos-x86_64 and r-oldrel-macos-x86_64**: the
  installation was interrupted at the builders' 30-minute limit
  (`make: Interrupt: 2` at ~1800 s, mid-compile of the `polyglot-sql` crate).
* **linux-arm64 additional issue**: rustc was killed for lack of memory while
  compiling the same crate with full LTO and a single codegen unit.

The Rust release profile is now `opt-level = "s"`, `lto = false` and
`codegen-units = 16`. This roughly halves the compile time and cuts the peak
memory of the largest crate by about a quarter: a local release install went
from about 15 minutes to 446 s wall clock (788 s CPU), and on macbuilder the
Rust build takes 6 minutes. On macOS and Linux the shared library exports only
its R entry point and drops local symbols, so the installed size stays at the
level of 0.1.1. The R API and results are unchanged.

I have no access to an x86_64 macOS builder with the CRAN time limit, so the
timing there can only be confirmed by the CRAN checks; the numbers above come
from the builds listed below.

The only other change is author metadata: the maintainer's name is now spelled
with its accent ("André Leite"; same person and e-mail address), a co-author's
surname and e-mail were corrected (Marcos Wasiliew), ORCID iDs were added and
Júlia Nascimento Barreto joins as author.

## Test environments

* local: macOS 26.6 (arm64), R 4.6.0, `R CMD check --as-cran`
* macbuilder: r-release (macOS 26.6 host, SDK 14.4, arm64) -- Status: OK,
  install in 371 s wall clock
* GitHub Actions: ubuntu-latest (R release, R oldrel-1), macos-latest
  (R release), windows-latest (R release)

## R CMD check results

0 errors | 0 warnings | 2 notes

* `Days since last update: 3` -- this release only fixes the check failures
  above.
* `checking HTML version of manual ... NOTE`: skipped because the local HTML
  Tidy is too old (local tooling, not a package issue).

`checking installed package size` reports (INFO) 48.5Mb, of which `libs` is
48.1Mb: a single shared object with the statically linked Rust engine, which
embeds complete tokenizers, parsers and code generators for 34 SQL dialects.
The size is executable code, not debugging information or data.

## Rust / compiled code notes

Following the CRAN policy on Rust packages:

* `SystemRequirements` declares `Cargo (Rust's package manager), rustc >=
  1.88.0, xz`. That minimum was determined empirically as the maximum
  `rust-version` declared by the vendored dependencies and verified by
  building the package with rustc 1.88.0.
* All Rust dependencies are vendored in `src/rust/vendor.tar.xz`, and cargo
  is invoked with `--offline`. **No code or binary is downloaded during
  `R CMD INSTALL`.**
* Compilation is limited to two parallel jobs (`cargo build -j 2`).
* `configure` and `configure.win` only *check* for cargo and rustc and print
  installation instructions; they never install a toolchain.
* Temporary build artefacts (`src/.cargo`, the unpacked `vendor` directory
  and the cargo target directory) are removed after the build, and `cleanup`
  removes the generated `src/Makevars`.
* The source tarball contains no compiled binaries.
* Licenses and copyright of the bundled Rust code, including the upstream
  Polyglot project (MIT) and the SQLGlot-derived portions (MIT), are recorded
  in `inst/COPYRIGHTS`, `inst/AUTHORS` and `Authors@R`.

## Downstream dependencies

There are currently no downstream dependencies.
