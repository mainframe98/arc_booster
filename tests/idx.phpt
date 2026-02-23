--TEST--
arc_booster: idx
--EXTENSIONS--
arc_booster
--FILE--
<?php

$array = array(
  'present' => true,
  'null' => null,
);

var_dump(ArcBooster\idx($array, 'present'));
var_dump(ArcBooster\idx($array, 'present', false));
var_dump(ArcBooster\idx($array, 'null'));
var_dump(ArcBooster\idx($array, 'null', false));
var_dump(ArcBooster\idx($array, 'missing'));
var_dump(ArcBooster\idx($array, 'missing', false));
var_dump(ArcBooster\idx($array, null, 5));

$array = array(
  '' => false,
  0 => '0',
);

var_dump(ArcBooster\idx($array, 0, false));
var_dump(ArcBooster\idx($array, null, true));
var_dump(ArcBooster\idx($array, true, 'boolean'));
var_dump(ArcBooster\idx($array, false, 'boolean'));
var_dump(ArcBooster\idx($array, 0.1, 'double'));
var_dump(ArcBooster\idx($array, "0", '0 as string'));

try {
  ArcBooster\idx($array, new stdClass);
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\idx((fn () => yield)(), 0);
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

?>
--EXPECTF--
bool(true)
bool(true)
NULL
NULL
NULL
bool(false)
int(5)
string(1) "0"
bool(false)
string(7) "boolean"
string(1) "0"

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s on line %d
string(1) "0"
string(1) "0"
TypeError: ArcBooster\idx(): Argument #2 ($key) must be a valid array offset type
TypeError: ArcBooster\idx(): Argument #1 ($array) must be of type array, Generator given
