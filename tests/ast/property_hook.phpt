--TEST--
arc_booster: AST manipulations for property hooks
--SKIPIF--
<?php
if (PHP_VERSION_ID < 80400) {
  die("skip PHP 8.4 or newer");
}
?>
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Hook {
  public string $test {
    get => idx($this->values, 'test', 'missing');
  }
}

echo ArcBooster\render_ast(file_get_contents(__FILE__));

?>
--EXPECT--

class Hook {
    public string $test {
        get => \ArcBooster\idx($this->values, 'test', 'missing');
    }
}

echo ArcBooster\render_ast(file_get_contents(__FILE__));
