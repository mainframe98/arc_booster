--TEST--
arc_booster: string_cast
--EXTENSIONS--
arc_booster
--FILE--
<?php

class StringableObject {
  public function __toString(): string {
    return __CLASS__;
  }
}

var_dump(ArcBooster\string_cast(123));
var_dump(ArcBooster\string_cast(null));
var_dump(ArcBooster\string_cast('text'));
var_dump(ArcBooster\string_cast(17.4));
var_dump(ArcBooster\string_cast(true));
var_dump(ArcBooster\string_cast(false));
var_dump(ArcBooster\string_cast(0));
var_dump(ArcBooster\string_cast(0.0));
var_dump(ArcBooster\string_cast(new StringableObject));

$a = 'by reference';
$b = &$a;
var_dump(ArcBooster\string_cast($b));

try {
  ArcBooster\string_cast(new stdClass);
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}
try {
  ArcBooster\string_cast(array());
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

?>
--EXPECT--
string(3) "123"
string(0) ""
string(4) "text"
string(4) "17.4"
string(1) "1"
string(0) ""
string(1) "0"
string(1) "0"
string(16) "StringableObject"
string(12) "by reference"
Error: Object of class stdClass could not be converted to string
TypeError: ArcBooster\string_cast(): Argument #1 ($value) must be convertable to string, array given
