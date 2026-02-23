--TEST--
arc_booster: ipull with ArrayAccess
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Accessor implements ArrayAccess {
  private $backing = array(
    'foo' => 'foo_value',
    'bar' => 'bar_value',
  );

  public function offsetExists(mixed $offset): bool { return isset($this->backing[$offset]); }
  public function offsetGet(mixed $offset): mixed { return $this->backing[$offset]; }
  public function offsetSet(mixed $offset, mixed $value): void { $this->backing[$offset] = $value; }
  public function offsetUnset(mixed $offset): void { unset($this->backing[$offset]); }
}

var_dump(ArcBooster\ipull(array(new Accessor), 'bar'));

?>
--EXPECT--
array(1) {
  [0]=>
  string(9) "bar_value"
}
