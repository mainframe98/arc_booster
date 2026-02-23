--TEST--
arc_booster: AST manipulations for idx
--EXTENSIONS--
arc_booster
--FILE--
<?php

$array = array(1, 2, 3, 4, 5, 6, 7, 8);

idx(array(), 'foo');
idx($array, 5);
idx($array, 10, false);
echo ArcBooster\render_ast(file_get_contents(__FILE__));

?>
--EXPECT--

$array = [1, 2, 3, 4, 5, 6, 7, 8];
[]['foo'] ?? \null;
$array[5] ?? \null;
\ArcBooster\idx($array, 10, false);
echo ArcBooster\render_ast(file_get_contents(__FILE__));
