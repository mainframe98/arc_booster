--TEST--
arc_booster: AST manipulations for id
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Test {
  public function foo() {
    return $this;
  }
}

id(new stdClass);
$var = id(new Test())->foo();
echo ArcBooster\render_ast(file_get_contents(__FILE__));

?>
--EXPECT--

class Test {
    public function foo() {
        return $this;
    }

}

new stdClass();
$var = new Test()->foo();
echo ArcBooster\render_ast(file_get_contents(__FILE__));
