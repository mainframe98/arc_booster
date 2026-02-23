--TEST--
arc_booster: mpull with stringable objects
--EXTENSIONS--
arc_booster
--FILE--
<?php

class StringableObject implements Stringable {
  public function __construct(private readonly string $string) {}

  public function __toString(): string {
    return $this->string;
  }
}

class Getter {
  public function __construct(private $key, private $value) {}

  public function key() { return $this->key; }
  public function value() { return $this->value; }
}

$array = array(
  new Getter(new StringableObject('foo'), new StringableObject('bar')),
);

try {
  ArcBooster\mpull($array, 'value', 'key');
} catch (Throwable $t) {
  echo $t::class, ': ', $t->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Cannot access offset of type StringableObject on array
