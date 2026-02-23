--TEST--
arc_booster: utf8v with invalid UTF-8
--EXTENSIONS--
arc_booster
--FILE--
<?php

try {
  ArcBooster\utf8v_codepoints("\xB9");
} catch (Throwable $e) {
  echo get_class($e).': '.$e->getMessage()."\n";
}

try {
  ArcBooster\utf8v_codepoints("\xFE");
} catch (Throwable $e) {
  echo get_class($e).': '.$e->getMessage()."\n";
}

try {
  ArcBooster\utf8v_codepoints("\xFB");
} catch (Throwable $e) {
  echo get_class($e).': '.$e->getMessage()."\n";
}

try {
  ArcBooster\utf8v_codepoints("\xDF\xC1");
} catch (Throwable $e) {
  echo get_class($e).': '.$e->getMessage()."\n";
}

?>
--EXPECT--
ValueError: Invalid UTF-8 string passed to ArcBooster\utf8v_codepoints.
ValueError: Invalid UTF-8 string passed to ArcBooster\utf8v_codepoints.
ValueError: Invalid UTF-8 string passed to ArcBooster\utf8v_codepoints.
ValueError: Invalid UTF-8 string passed to ArcBooster\utf8v_codepoints.
