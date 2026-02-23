--TEST--
GH-24523 (UAF in zend_test_compile_to_ast() on parse error)
--EXTENSIONS--
arc_booster
--FILE--
<?php

try {
    ArcBooster\render_ast("<?php (");
} catch (ParseError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Unclosed '('
