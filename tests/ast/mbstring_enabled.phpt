--TEST--
arc_booster: AST manipulations for mbstring
--EXTENSIONS--
arc_booster
mbstring
--INI--
arc_booster.use_mbstring=1
--FILE--
<?php

phutil_is_utf8('test');
phutil_utf8_strlen('test');

echo ArcBooster\render_ast(file_get_contents(__FILE__));

?>
--EXPECT--

\mb_check_encoding((string)'test', 'UTF-8');
\mb_strlen('test', 'UTF-8');
echo ArcBooster\render_ast(file_get_contents(__FILE__));
