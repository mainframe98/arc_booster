<?php

/** @generate-function-entries */

/**
 * ================================================================================================
 * | USE arc_booster_stub.php INSTEAD IF YOU ARE LOOKING FOR DOCUMENTATION OR STUBS FOR YOUR IDE. |
 * ================================================================================================
 *
 * This is a stub file meant only for use with https://github.com/php/php-src/blob/master/build/gen_stub.php
 * to generate Reflection information (ReflectionParameter, ReflectionFunction, ReflectionMethod, etc.)
 */

namespace ArcBooster;

/** @compile-time-eval */
function idx(array $array, string|bool|int|float|null $key, mixed $default = null): mixed {}

/** @compile-time-eval */
function string_cast(string|bool|int|float|\Stringable|null $value): string {}

/** @compile-time-eval */
function nonempty_string(string|null $value): bool {}

/** @compile-time-eval */
function nonempty_stringlike(string|\Stringable|null $value): bool {}

/** @compile-time-eval */
function nonempty_scalar(string|bool|int|float|\Stringable|null $value): bool {}

function coalesce(mixed ...$args): mixed {}

function nonempty(mixed ...$args): mixed {}

function mpull(iterable $list, ?string $method, ?string $key_method = null): array {}

function ppull(iterable $list, ?string $property, ?string $key_property = null): array {}

function ipull(iterable $list, string|bool|int|float|null $index, string|bool|int|float|null $key_index = null): array {}

/** @compile-time-eval */
function head(array $arr): mixed {}

/** @compile-time-eval */
function last(array $arr): mixed {}

/** @compile-time-eval */
function array_mergev(array $arrayv): array {}

/** @compile-time-eval */
function array_fuse(array|null $list = null): array {}

function is_utf8_slowly(string|null $string, bool $only_bmp = false): bool {}

function utf8v(string|null $string, int|null $byte_limit = null): array {}

function utf8v_codepoints(string|null $string): array {}

function utf8ize(string|null $string): string {}

function render_ast(string $source_code): string {}
