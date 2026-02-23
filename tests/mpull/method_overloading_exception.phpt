--TEST--
arc_booster: mpull with __call that throws exception
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Getter {
  public function __call(string $name, array $arguments): mixed {
    throw new Exception('Bad method');
  }
}

try {
  ArcBooster\mpull(array(new Getter), 'virtual');
} catch (Throwable $e) {
  echo get_class($e).': '.$e->getMessage()."\n";
}

?>
--EXPECT--
Exception: Bad method
