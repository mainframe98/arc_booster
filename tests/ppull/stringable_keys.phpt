--TEST--
arc_booster: ppull with stringable objects
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
  public function __construct(public $key, public $value) {}
}

$array = array(
  new Getter(new StringableObject('foo'), new StringableObject('bar')),
);

try {
  ArcBooster\ppull($array, 'value', 'key');
} catch (Throwable $t) {
  echo $t::class, ': ', $t->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Cannot access offset of type StringableObject on array
