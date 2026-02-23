--TEST--
arc_booster: array_mergev invalid values
--EXTENSIONS--
arc_booster
--FILE--
<?php

try {
  ArcBooster\array_mergev(array(null));
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\array_mergev(array(false));
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\array_mergev(array(true));
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\array_mergev(array('test'));
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\array_mergev(array(new stdClass));
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\array_mergev(array(new ArrayObject));
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\array_mergev(array(1));
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\array_mergev(array(1.5));
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\array_mergev(array(array(1, 2, 3, 4), null));
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

try {
  ArcBooster\array_mergev(array('foo' => array(1, 2, 3, 4), 'bar' => null));
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

?>
--EXPECT--
InvalidArgumentException: Offset #0 must be of type array, null given
InvalidArgumentException: Offset #0 must be of type array, false given
InvalidArgumentException: Offset #0 must be of type array, true given
InvalidArgumentException: Offset #0 must be of type array, string given
InvalidArgumentException: Offset #0 must be of type array, stdClass given
InvalidArgumentException: Offset #0 must be of type array, ArrayObject given
InvalidArgumentException: Offset #0 must be of type array, int given
InvalidArgumentException: Offset #0 must be of type array, float given
InvalidArgumentException: Offset #1 must be of type array, null given
InvalidArgumentException: Offset "bar" must be of type array, null given
