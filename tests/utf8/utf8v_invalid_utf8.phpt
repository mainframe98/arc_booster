--TEST--
arc_booster: utf8v with invalid UTF-8
--EXTENSIONS--
arc_booster
--FILE--
<?php

try {
  ArcBooster\utf8v("\xB9");
} catch (Throwable $e) {
  echo get_class($e).': '.$e->getMessage()."\n";
}

try {
  ArcBooster\utf8v("\xFE");
} catch (Throwable $e) {
  echo get_class($e).': '.$e->getMessage()."\n";
}

try {
  ArcBooster\utf8v("\xFB");
} catch (Throwable $e) {
  echo get_class($e).': '.$e->getMessage()."\n";
}

try {
  ArcBooster\utf8v("\xDF\xC1");
} catch (Throwable $e) {
  echo get_class($e).': '.$e->getMessage()."\n";
}

?>
--EXPECT--
ValueError: Invalid UTF-8 string passed to ArcBooster\utf8v.
ValueError: Invalid UTF-8 string passed to ArcBooster\utf8v.
ValueError: Invalid UTF-8 string passed to ArcBooster\utf8v.
ValueError: Invalid UTF-8 string passed to ArcBooster\utf8v.
