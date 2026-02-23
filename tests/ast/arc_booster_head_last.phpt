--TEST--
arc_booster: AST manipulations for head/last
--EXTENSIONS--
arc_booster
--FILE--
<?php

$array = [1, 2, 3];
head($array);
last($array);

echo ArcBooster\render_ast(file_get_contents(__FILE__));

?>
--EXPECT--
$array = [1, 2, 3];
\ArcBooster\head($array);
\ArcBooster\last($array);
echo ArcBooster\render_ast(file_get_contents(__FILE__));
