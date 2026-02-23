--TEST--
arc_booster: ppull with __get that throws exception
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Accessor {
  public function __isset(string $name): bool {
    return true;
  }
  public function __get(string $name): mixed {
    throw new Exception('Bad property');
  }
}

try {
  ArcBooster\ppull(array(new Accessor), 'virtual');
} catch (Throwable $e) {
  echo get_class($e).": ".$e->getMessage()."\n";
}

?>
--EXPECT--
Exception: Bad property
