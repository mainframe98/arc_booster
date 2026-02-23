--TEST--
arc_booster: ipull with ArrayAccess that throws exception
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Accessor implements ArrayAccess {
  public function offsetExists(mixed $offset): bool { throw new Exception(__METHOD__); }
  public function offsetGet(mixed $offset): mixed { throw new Exception(__METHOD__); }
  public function offsetSet(mixed $offset, mixed $value): void {}
  public function offsetUnset(mixed $offset): void {}
}

try {
  ArcBooster\ipull(array(new Accessor), 'virtual');
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

?>
--EXPECT--
Exception: Accessor::offsetGet
