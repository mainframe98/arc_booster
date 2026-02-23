--TEST--
arc_booster: mpull array iteration with invalid callables
--EXTENSIONS--
arc_booster
--FILE--
<?php

try {
  ArcBooster\mpull(array(new stdClass), '');
} catch (Throwable $e) {
  echo get_class($e).': '.$e->getMessage()."\n";
}

try {
  ArcBooster\mpull(array(null), 'test');
} catch (Throwable $e) {
  echo get_class($e).': '.$e->getMessage()."\n";
}

?>
--EXPECT--
Error: Call to undefined method stdClass::()
Error: Call to a member function test() on null
