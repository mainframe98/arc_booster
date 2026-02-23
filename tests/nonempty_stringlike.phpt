--TEST--
arc_booster: nonempty_stringlike
--EXTENSIONS--
arc_booster
--FILE--
<?php

class StringableObject {
  public function __toString(): string {
    return __CLASS__;
  }
}

var_dump(ArcBooster\nonempty_stringlike(''));
var_dump(ArcBooster\nonempty_stringlike(null));
var_dump(ArcBooster\nonempty_stringlike('text'));
var_dump(ArcBooster\nonempty_stringlike(new StringableObject));

try {
  ArcBooster\nonempty_stringlike(array());
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
TypeError: ArcBooster\nonempty_stringlike(): Argument #1 ($value) must be of type ?string, array given
