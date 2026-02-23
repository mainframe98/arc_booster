--TEST--
arc_booster: mpull with __call
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Getter {
  public function __construct(private $prop) {}

  public function __call(string $name, array $arguments): mixed {
    if ($name === 'getProp') {
      return $this->prop;
    }
    throw new Exception('Bad method');
  }
}

var_dump(ArcBooster\mpull(array(new Getter('foo')), 'getProp'));

?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "foo"
}
