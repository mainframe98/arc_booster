--TEST--
arc_booster: AST manipulations for mbstring
--EXTENSIONS--
arc_booster
--INI--
arc_booster.use_mbstring=0
--FILE--
<?php

function phutil_is_utf8(string $s) {}
function phutil_utf8_strlen(string $s) {}

phutil_is_utf8('test');
phutil_utf8_strlen('test');

echo ArcBooster\render_ast(file_get_contents(__FILE__));

?>
--EXPECT--

function phutil_is_utf8(string $s) {
}

function phutil_utf8_strlen(string $s) {
}

\ArcBooster\is_utf8_slowly('test');
phutil_utf8_strlen('test');
echo ArcBooster\render_ast(file_get_contents(__FILE__));
