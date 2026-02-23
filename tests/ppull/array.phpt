--TEST--
arc_booster: ppull array
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Getter {
  public function __construct(public $key, public $value) {}
}

$array = array();

foreach (range(1, 5) as $i) {
  $array[$i] = new Getter("key $i", "value $i");
}

$res = ArcBooster\ppull($array, 'value');

foreach ($res as $key => $value) {
  echo "'$key': '$value'\n";
}

$res = ArcBooster\ppull($array, 'value', 'key');

foreach ($res as $key => $value) {
  echo "'$key': '$value'\n";
}

$array = array(
  5 => new Getter("key 5", "value 5"),
  7 => new Getter("key 7", "value 7"),
  9 => new Getter("key 9", "value 9"),
);

$res = ArcBooster\ppull($array, 'value', 'key');

foreach ($res as $key => $value) {
  echo "'$key': '$value'\n";
}

$res = ArcBooster\ppull($array, 'value');

foreach ($res as $key => $value) {
  echo "'$key': '$value'\n";
}

?>
--EXPECT--
'1': 'value 1'
'2': 'value 2'
'3': 'value 3'
'4': 'value 4'
'5': 'value 5'
'key 1': 'value 1'
'key 2': 'value 2'
'key 3': 'value 3'
'key 4': 'value 4'
'key 5': 'value 5'
'key 5': 'value 5'
'key 7': 'value 7'
'key 9': 'value 9'
'5': 'value 5'
'7': 'value 7'
'9': 'value 9'
