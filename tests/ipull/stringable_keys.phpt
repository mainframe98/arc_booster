--TEST--
arc_booster: ipull with stringable objects
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

$array = array(
  array('key' => new StringableObject('foo'), 'value' => new StringableObject('bar')),
);

try {
  ArcBooster\ipull($array, 'value', 'key');
} catch (Throwable $t) {
  echo $t::class, ': ', $t->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Cannot access offset of type StringableObject on array
