# Changelog

## polyglotSQL 0.1.3

- Updated the embedded `polyglot-sql` engine from 0.12.0 to 0.13.2
  (upstream changelog:
  <https://github.com/tobilg/polyglot/blob/main/CHANGELOG.md>).
  Highlights inherited by the R API: two new dialects, `"hana"` (SAP
  HANA, aliases `"saphana"`/`"sap_hana"`) and `"vertica"`, bringing
  [`sql_dialects()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_dialects.md)
  to 36 entries; set-operation output types
  (`UNION`/`INTERSECT`/`EXCEPT`, including `BY NAME` and
  `CORRESPONDING`) are now resolved by
  [`sql_analyze()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_analyze.md)
  and the type annotation; Oracle output renders `LIMIT` as
  `OFFSET ... FETCH FIRST ... ROWS ONLY` and compound queries keep a
  trailing `FETCH FIRST`; branch-local row limits inside set operations
  are wrapped in parentheses instead of limiting the whole union; DuckDB
  `//` integer division and `ORDER BY ALL` translate correctly; BigQuery
  named parameters, decimal precision, interval arithmetic and
  `DATE(timestamp, time_zone)` convert to DuckDB faithfully; Snowflake
  `PIVOT`/`UNPIVOT` over CTEs no longer loops in lineage analysis;
  schema validation covers `VALUES` sources, `LATERAL` and
  `INSERT ... SELECT`.
- On macOS x86_64 (Intel), the embedded engine crate is now compiled
  without optimization whenever `NOT_CRAN` is unset, which is how the
  CRAN binaries are built (`configure`/`tools/config.R`). The CRAN
  builders for that platform stop an installation after 30 minutes and
  interrupted the 0.1.1 and 0.1.2 builds. The build is about five times
  faster; the engine runs 2-4 times slower there, which is still a few
  milliseconds per call, and the shared object is larger.
  `NOT_CRAN=true R CMD INSTALL .` gives the fully optimized build. Other
  platforms are unchanged.

## polyglotSQL 0.1.2

CRAN release: 2026-10-08

- Lighter Rust release profile (`opt-level = "s"`, no LTO, 16 codegen
  units) to cut the cost of building from source: about half the compile
  time and roughly a quarter less peak memory for the largest crate.
  This targets the CRAN macOS x86_64 builders, which stopped the
  installation at their 30-minute limit, and the linux-arm64 check,
  where the compiler ran out of memory. On macOS and Linux the shared
  library now exports only its R entry point and leaves out local
  symbols, which keeps the installed size at the level of 0.1.1. The R
  API and results are unchanged.
- On Linux and macOS the shared object no longer references the C
  library’s `abort()`. The Rust standard library calls it as a last
  resort (a failed memory allocation, a panic raised while another panic
  is being handled); those calls are now redirected at link time to a
  function that raises an R error instead, so compiled code can never
  terminate the R session. This removes the `Found 'abort'` NOTE of the
  CRAN linux-arm64 check. Ordinary Rust panics were already caught and
  turned into R errors.

## polyglotSQL 0.1.1

CRAN release: 2026-09-27

- Updated the embedded `polyglot-sql` engine from 0.6.2 to 0.12.0
  (upstream changelog:
  <https://github.com/tobilg/polyglot/blob/main/CHANGELOG.md>).
  Highlights inherited by the R API: parser-depth protection enabled by
  default (deeply nested SQL now raises a `polyglot_guard_error` instead
  of risking a stack overflow); grouping, aggregate and window placement
  errors in semantic validation (`E230`-`E232`);
  [`sql_analyze()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_analyze.md)
  now reports scoped non-projection `columnUses` and propagates
  nullability and cast types through CTEs and derived tables;
  `NULLS FIRST`/`NULLS LAST` are preserved when formatting; 128-bit and
  unsigned integer types; name-aligned `UNION BY NAME` handling; many
  dialect fixes (DuckDB, Snowflake, ClickHouse, BigQuery, T-SQL).
- `sql_validate(semantic = TRUE)` now also reports semantic correctness
  errors `E230`-`E232` (ungrouped columns, misplaced aggregates and
  window functions), which invalidate the statement; quality hints
  remain warnings. Default syntax-only validation is unchanged.
- Errors of the new upstream kinds `invalid_input` and
  `column_resolution` are surfaced as plain `polyglot_error` conditions.
- macOS: the Rust build now uses the deployment target of R’s C compiler
  (including a `-mmacos-version-min` flag set in `CC`) instead of the
  version of the running system. This removes the `ld` warning “object
  file was built for newer ‘macOS’ version than being linked” seen in
  the CRAN M1mac checks.

## polyglotSQL 0.1.0

CRAN release: 2026-08-04

Initial release, wrapping the `polyglot-sql` Rust crate 0.6.2.

- Core:
  [`polyglot_version()`](https://strategicprojects.github.io/polyglot-sql-r/reference/polyglot_version.md),
  [`sql_dialects()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_dialects.md)
  (34 dialects with aliases).
- Transpilation and formatting:
  [`sql_transpile()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_transpile.md),
  [`sql_format()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_format.md),
  [`sql_generate()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_generate.md)
  with complexity guards and configurable handling of unsupported
  constructs.
- Parsing and validation:
  [`sql_parse()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_parse.md),
  [`sql_tokenize()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_tokenize.md),
  [`sql_validate()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_validate.md)
  (strict syntax, semantic warnings, schema-aware checks).
- Analysis and lineage:
  [`sql_source_tables()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_source_tables.md),
  [`sql_lineage()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_lineage.md),
  [`sql_analyze()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_analyze.md),
  [`sql_optimize()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_optimize.md),
  [`sql_diff()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_diff.md),
  [`sql_annotate_types()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_annotate_types.md),
  [`sql_openlineage()`](https://strategicprojects.github.io/polyglot-sql-r/reference/sql_openlineage.md).
- Classed error conditions (`polyglot_error`, `polyglot_parse_error`,
  `polyglot_transpile_error`, `polyglot_validation_error`,
  `polyglot_guard_error`, …); Rust failures never crash the R session.
- Fully offline source builds via vendored crates (`cargo vendor`).
