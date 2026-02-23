--TEST--
arc_booster: AST manipulations for the pipe operator
--SKIPIF--
<?php
if (PHP_VERSION_ID < 80500) {
  die("skip PHP 8.5 or newer");
}
?>
--EXTENSIONS--
arc_booster
--FILE--
<?php

$array = array(1, 2, 3);

$res = $array
  |> array_keys(...)
  |> phutil_is_natural_list(...);

echo ArcBooster\render_ast(file_get_contents(__FILE__));

--EXPECT--

$array = [1, 2, 3];
$res = $array |> array_keys(...) |> \array_is_list(...);
echo ArcBooster\render_ast(file_get_contents(__FILE__));
