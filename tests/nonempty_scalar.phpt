--TEST--
arc_booster: nonempty_scalar
--EXTENSIONS--
arc_booster
--FILE--
<?php

class StringableObject {
  public function __toString(): string {
    return __CLASS__;
  }
}

class ExceptionThrowingObject {
  public function __toString(): string {
    throw new Exception(__FUNCTION__);
  }
}

var_dump(ArcBooster\nonempty_scalar(''));
var_dump(ArcBooster\nonempty_scalar(null));
var_dump(ArcBooster\nonempty_scalar('text'));
var_dump(ArcBooster\nonempty_scalar(false));
var_dump(ArcBooster\nonempty_scalar(true));
var_dump(ArcBooster\nonempty_scalar(0));
var_dump(ArcBooster\nonempty_scalar(0.0));
var_dump(ArcBooster\nonempty_scalar(1));
var_dump(ArcBooster\nonempty_scalar(1.1));
var_dump(ArcBooster\nonempty_scalar(new StringableObject));

try {
  ArcBooster\nonempty_scalar(new stdClass);
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\nonempty_scalar(array());
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\nonempty_scalar(new ExceptionThrowingObject);
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
TypeError: ArcBooster\nonempty_scalar(): Argument #1 ($value) must be a scalar, stdClass given
TypeError: ArcBooster\nonempty_scalar(): Argument #1 ($value) must be a scalar, array given
Exception: __toString
