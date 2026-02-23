--TEST--
arc_booster: AST manipulations for first class callables
--EXTENSIONS--
arc_booster
--FILE--
<?php

head_key(...);
last_key(...);
phutil_fwrite_nonblocking_stream(...);
phutil_hashes_are_identical(...);
phutil_is_natural_list(...);
idx(...);
phutil_string_cast(...);
phutil_nonempty_string(...);
phutil_nonempty_scalar(...);
array_mergev(...);
phutil_utf8ize(...);
phutil_utf8v_codepoints(...);
phutil_utf8v(...);
phutil_is_utf8_slowly(...);

echo ArcBooster\render_ast(file_get_contents(__FILE__));

?>
--EXPECTF--

\array_key_first(...);
\array_key_last(...);
\fwrite(...);
\hash_equals(...);
\array_is_list(...);
\ArcBooster\idx(...);
\ArcBooster\string_cast(...);
\ArcBooster\nonempty_string(...);
\ArcBooster\nonempty_scalar(...);
\ArcBooster\array_mergev(...);
\ArcBooster\utf8ize(...);
\ArcBooster\utf8v_codepoints(...);
\ArcBooster\utf8v(...);
\ArcBooster\is_utf8_slowly(...);
echo ArcBooster\render_ast(file_get_contents(__FILE__));
