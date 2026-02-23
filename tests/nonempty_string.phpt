--TEST--
arc_booster: nonempty_string
--EXTENSIONS--
arc_booster
--FILE--
<?php

class StringableObject {
  public function __toString(): string {
    return __CLASS__;
  }
}

var_dump(ArcBooster\nonempty_string(''));
var_dump(ArcBooster\nonempty_string(null));
var_dump(ArcBooster\nonempty_string('text'));

try {
  ArcBooster\nonempty_string(new StringableObject);
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\nonempty_string(array());
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
TypeError: ArcBooster\nonempty_string(): Argument #1 ($value) must be of type ?string, StringableObject given
TypeError: ArcBooster\nonempty_string(): Argument #1 ($value) must be of type ?string, array given
