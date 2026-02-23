--TEST--
arc_booster: mpull using objects as values
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Getter {
  public function __construct(private $key, private $value) {}

  public function key() { return $this->key; }
  public function value() { return $this->value; }
}

$array = array(
  new Getter(new stdClass, new StdClass),
  new Getter(new stdClass, new StdClass),
  new Getter(new stdClass, new StdClass),
  new Getter(new stdClass, new StdClass),
);

$res = ArcBooster\mpull($array, 'value');

var_dump($res);

?>
--EXPECT--
array(4) {
  [0]=>
  object(stdClass)#3 (0) {
  }
  [1]=>
  object(stdClass)#6 (0) {
  }
  [2]=>
  object(stdClass)#9 (0) {
  }
  [3]=>
  object(stdClass)#12 (0) {
  }
}
