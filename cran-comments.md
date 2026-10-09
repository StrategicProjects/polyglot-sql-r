## Submission

polyglotSQL 0.1.3 updates the embedded `polyglot-sql` Rust engine from 0.12.0
to 0.13.2 (two new dialects, SAP HANA and Vertica; set-operation typing,
Oracle/T-SQL row-limit rendering and a number of transpilation fixes, see
NEWS.md) and addresses the remaining check failure of 0.1.2:

* **ERROR on r-release-macos-x86_64 and r-oldrel-macos-x86_64**: the
  installation is interrupted at the builders' 30-minute limit
  (`make: Interrupt: 2` at ~1800 s, mid-compile of the `polyglot-sql` crate).
  The same build completes in 12 minutes on r-release-macos-arm64 and within
  the limits of every other flavour. On those two flavours only (`configure`
  detects a CRAN build on x86_64 macOS), the engine crate is now compiled
  without optimization (and with `debug_assertions`, which the crate uses to
  size its stack red zone for unoptimized frames): locally that build takes
  120 s instead of 662 s wall
  clock with two jobs (`-j 2`), so it should fit comfortably even on the
  slower x86_64 hosts. The engine runs 2-4 times slower there (about 1-7 ms
  per call on the benchmark queries instead of 0.2-3 ms) and the shared
  object is about twice as large. All other platforms get the same optimized
  build as 0.1.2. I still have no access to an x86_64 macOS builder with the
  CRAN time limit, so the timing there can only be confirmed by the CRAN
  checks.

## Test environments

* local: macOS 26.6 (arm64), R 4.6.0, `R CMD check --as-cran`
* GitHub Actions: ubuntu-latest (R release, R oldrel-1), macos-latest
  (R release), windows-latest (R release)

## R CMD check results

0 errors | 0 warnings | 1 note

* `checking HTML version of manual ... NOTE`: skipped because the local HTML
  Tidy is too old (local tooling, not a package issue).

`checking installed package size` reports (INFO) 53.9 Mb, almost all of it
`libs`: a single shared object with the statically linked Rust engine, which
embeds complete tokenizers, parsers and code generators for 36 SQL dialects.
The size is executable code, not debugging information or data. On the macOS
x86_64 builders (unoptimized engine, see above) it is about 100 Mb.

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
* On Linux and macOS the shared object does not reference `abort()` (the
  Rust standard library's last-resort calls are redirected at link time to a
  function that raises an R error; see `tools/config.R`), as requested for
  0.1.2.
* Licenses and copyright of the bundled Rust code, including the upstream
  Polyglot project (MIT) and the SQLGlot-derived portions (MIT), are recorded
  in `inst/COPYRIGHTS`, `inst/AUTHORS` and `Authors@R`.

## Downstream dependencies

There are currently no downstream dependencies.
