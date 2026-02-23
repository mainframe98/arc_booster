--TEST--
arc_booster: ipull array iteration without key or value index
--EXTENSIONS--
arc_booster
--FILE--
<?php

function generate() {
  foreach (range(1, 5) as $i) {
    yield $i => array('key' => "key $i", 'value' => "value $i");
  }
}
$res = ArcBooster\mpull(generate(), null, null);

var_dump(iterator_to_array(generate()) == $res);

?>
--EXPECT--
bool(true)
