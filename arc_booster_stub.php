<?php

namespace ArcBooster;

/**
 * Access an array by index. If the array does not have the index, the default is returned.
 * Using `null` for the key will be treated as an empty string. This will not issue a deprecation warning.
 *
 * @template T
 * @template D
 *
 * @param array<int|string, T> $array
 * @param scalar $key
 * @param D|null $default
 * @return T|D
 */
function idx(array $array, string|bool|int|float|null $key, mixed $default = null): mixed {}

/**
 * Like strval, but throws an exception when given an array.
 */
function string_cast(string|bool|int|float|\Stringable|null $value): string {}

function nonempty_string(string|null $value): bool {}

function nonempty_stringlike(string|\Stringable|null $value): bool {}

function nonempty_scalar(string|bool|int|float|\Stringable|null $value): bool {}

/**
 * Returns the first argument which is not strictly null, or `null` if there are no such arguments.
 *
 * @param mixed ...$args Zero or more arguments of any type.
 * @return mixed First non-`null` arg, or null if no such argument exists.
 */
function coalesce(mixed ...$args): mixed {}

/**
 * Returns the first non-`empty()` argument, instead of the first argument that is strictly non-`null`.
 * If no argument is nonempty, it returns the last argument.
 *
 * @param mixed ...$args Zero or more arguments of any type.
 * @return mixed First non-`empty()` argument, or last argument if no such argument exists, or null if given zero arguments.
 */
function nonempty(mixed ...$args): mixed {}

function mpull(iterable $list, ?string $method, ?string $key_method = null): array {}

function ppull(iterable $list, ?string $property, ?string $key_property = null): array {}

function ipull(iterable $list, string|bool|int|float|null $index, string|bool|int|float $key_index = null): array {}

/**
 * @template T
 *
 * @param array<T> $arr
 * @return T|false
 */
function head(array $arr): mixed {}

/**
 * @template T
 *
 * @param array<T> $arr
 * @return T|false
 */
function last(array $arr): mixed {}

function array_mergev(array $arrayv): array {}

/**
 * Like array_combine, but uses the same array for both keys and values.
 */
function array_fuse(array|null $list = null): array {}

/**
 * Determine if a string is valid UTF-8. Despite the name, this function isn't slow.
 * The equivalent Arcanist function is, however, and this function takes its name from there.
 *
 * @param string|null $string Some string which may or may not be valid UTF-8.
 * @param bool $only_bmp (optional) True to require all characters be part of the basic multilingual plane (no more than 3-bytes long).
 * @return bool True if the string is valid UTF-8.
 */
function is_utf8_slowly(string|null $string, bool $only_bmp = false): bool {}

/**
 * Split a UTF-8 string into an array of characters. Combining characters are also split.
 *
 * @param string|null $string A valid utf-8 string.
 * @param int|null $byte_limit Stop processing after examining this many bytes.
 * @return string[] A list of characters in the string.
 */
function utf8v(string|null $string, int|null $byte_limit = null): array {}

/**
 * Split a UTF-8 string into an array of codepoints (as integers).
 *
 * @param string|null $string A valid UTF-8 string.
 * @return int[] A list of codepoints, as integers.
 */
function utf8v_codepoints(string|null $string): array {}

/**
 * Convert a string into valid UTF-8.
 *
 * When invalid byte subsequences are encountered, they will be replaced with
 * U+FFFD, the Unicode replacement character.
 * Note that incomplete sequences are considered multiple subsequences.
 *
 * This function treats overlong encodings as invalid.
 *
 * @param string|null $string String to convert to valid UTF-8.
 * @return string String with invalid UTF-8 byte subsequences replaced with U+FFFD.
 */
function utf8ize(string|null $string): string {}

/**
 * Renders the AST that is generated for the given PHP source code. This will perform ArcBooster AST modifications, even when arc_booster.disable_ast_modifications is true.
 *
 * @param string $source Source code
 * @param string
 */
function render_ast(string $source): string {}
