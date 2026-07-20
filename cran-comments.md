## Submission

This is a new submission of polyglotSQL 0.1.0.

polyglotSQL provides an R interface to the `polyglot-sql` Rust crate for
parsing, validating, formatting and translating SQL between more than 30
dialects. All computation happens in-process; the package makes no network
requests and requires no database, Python or Java runtime.

## Test environments

* local macOS 26 (arm64), R 4.6.0
* GitHub Actions: ubuntu-latest (R release, R oldrel-1), macOS-latest
  (R release), windows-latest (R release)
* win-builder (R-devel)
* macOS builder (R-release, arm64)

## R CMD check results

0 errors | 0 warnings | 1 note

The note is the expected size note for a package with a compiled Rust
backend:

```
* checking installed package size ... NOTE
  installed size is 46.5Mb
  sub-directories of 1Mb or more:
    libs  46.1Mb
```

The `libs` directory contains a single shared object with the statically
linked Rust engine. The size reflects genuine functionality: the package
embeds complete tokenizers, parsers and code generators for 34 SQL dialects,
which is the core purpose of the package. The release profile already uses
link-time optimization and a single codegen unit; the remaining size is
executable code, not debugging information or data.

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

There are currently no downstream dependencies (new submission).
