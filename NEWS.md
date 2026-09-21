# polyglotSQL 0.1.1

* Updated the embedded `polyglot-sql` engine from 0.6.2 to 0.12.0 (upstream
  changelog: <https://github.com/tobilg/polyglot/blob/main/CHANGELOG.md>).
  Highlights inherited by the R API: parser-depth protection enabled by
  default (deeply nested SQL now raises a `polyglot_guard_error` instead of
  risking a stack overflow); grouping, aggregate and window placement errors
  in semantic validation (`E230`-`E232`); `sql_analyze()` now reports scoped
  non-projection `columnUses` and propagates nullability and cast types
  through CTEs and derived tables; `NULLS FIRST`/`NULLS LAST` are preserved
  when formatting; 128-bit and unsigned integer types; name-aligned
  `UNION BY NAME` handling; many dialect fixes (DuckDB, Snowflake,
  ClickHouse, BigQuery, T-SQL).
* `sql_validate(semantic = TRUE)` now also reports semantic correctness
  errors `E230`-`E232` (ungrouped columns, misplaced aggregates and window
  functions), which invalidate the statement; quality hints remain warnings.
  Default syntax-only validation is unchanged.
* Errors of the new upstream kinds `invalid_input` and `column_resolution`
  are surfaced as plain `polyglot_error` conditions.
* macOS: the Rust build now uses the deployment target of R's C compiler
  (including a `-mmacos-version-min` flag set in `CC`) instead of the version
  of the running system. This removes the `ld` warning "object file was built
  for newer 'macOS' version than being linked" seen in the CRAN M1mac checks.

# polyglotSQL 0.1.0

Initial release, wrapping the `polyglot-sql` Rust crate 0.6.2.

* Core: `polyglot_version()`, `sql_dialects()` (34 dialects with aliases).
* Transpilation and formatting: `sql_transpile()`, `sql_format()`,
  `sql_generate()` with complexity guards and configurable handling of
  unsupported constructs.
* Parsing and validation: `sql_parse()`, `sql_tokenize()`, `sql_validate()`
  (strict syntax, semantic warnings, schema-aware checks).
* Analysis and lineage: `sql_source_tables()`, `sql_lineage()`,
  `sql_analyze()`, `sql_optimize()`, `sql_diff()`, `sql_annotate_types()`,
  `sql_openlineage()`.
* Classed error conditions (`polyglot_error`, `polyglot_parse_error`,
  `polyglot_transpile_error`, `polyglot_validation_error`,
  `polyglot_guard_error`, ...); Rust failures never crash the R session.
* Fully offline source builds via vendored crates (`cargo vendor`).
