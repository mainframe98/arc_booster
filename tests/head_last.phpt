--TEST--
arc_booster: ArcBooster\head()/ArcBooster\last()
--EXTENSIONS--
arc_booster
--FILE--
<?php
$str = "hello world";

$test_cases = [
    ["single element"],
    [&$str, 1],
    [1, &$str],
    [1 => 1, 0 => 0, 3 => 3, 2 => 2],
    [100 => []],
    [new stdClass, false],
    [true, new stdClass],
];

foreach ($test_cases as $test_case) {
    // Output the checked values
    echo "--- Testing: ", json_encode($test_case), " ---\n";
    echo "First: ", json_encode(ArcBooster\head($test_case)), "\n";
    echo "Last: ", json_encode(ArcBooster\last($test_case)), "\n";

    // Sanity check consistency with array_key_first()/array_key_last()
    if (ArcBooster\head($test_case) !== $test_case[array_key_first($test_case)]) {
        throw new Error("Key first and value first inconsistency");
    }
    if (ArcBooster\last($test_case) !== $test_case[array_key_last($test_case)]) {
        throw new Error("Key last and value last inconsistency");
    }
}

var_dump(ArcBooster\head(array()));
var_dump(ArcBooster\last(array()));

?>
--EXPECT--
--- Testing: ["single element"] ---
First: "single element"
Last: "single element"
--- Testing: ["hello world",1] ---
First: "hello world"
Last: 1
--- Testing: [1,"hello world"] ---
First: 1
Last: "hello world"
--- Testing: {"1":1,"0":0,"3":3,"2":2} ---
First: 1
Last: 2
--- Testing: {"100":[]} ---
First: []
Last: []
--- Testing: [{},false] ---
First: {}
Last: false
--- Testing: [true,{}] ---
First: true
Last: {}
bool(false)
bool(false)
