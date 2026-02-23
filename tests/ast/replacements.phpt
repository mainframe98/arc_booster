--TEST--
arc_booster: AST manipulations: Wholesale function replacements
--EXTENSIONS--
arc_booster
--FILE--
<?php

head_key(array('first' => true, 'last' => false));
last_key(array('first' => false, 'last' => true));
phutil_fwrite_nonblocking_stream(STDERR, '');
phutil_hashes_are_identical('', '');
phutil_is_natural_list(array());
idx(array(), 5, false);
phutil_string_cast(5);
phutil_nonempty_string('test');
phutil_nonempty_scalar('test');
coalesce();
nonempty();
array_mergev(array());
array_fuse(array());
phutil_is_utf8_slowly('');
phutil_is_utf8_slowly('', true);
phutil_utf8ize('');
phutil_utf8v_codepoints('');
phutil_utf8v('');

echo ArcBooster\render_ast(file_get_contents(__FILE__));

?>
--EXPECT--

\array_key_first(['first' => true, 'last' => false]);
\array_key_last(['first' => false, 'last' => true]);
\fwrite(STDERR, '');
\hash_equals('', '');
\array_is_list([]);
\ArcBooster\idx([], 5, false);
\ArcBooster\string_cast(5);
\ArcBooster\nonempty_string('test');
\ArcBooster\nonempty_scalar('test');
\ArcBooster\coalesce();
\ArcBooster\nonempty();
\ArcBooster\array_mergev([]);
\ArcBooster\array_fuse([]);
\ArcBooster\is_utf8_slowly('');
\ArcBooster\is_utf8_slowly('', true);
\ArcBooster\utf8ize('');
\ArcBooster\utf8v_codepoints('');
\ArcBooster\utf8v('');
echo ArcBooster\render_ast(file_get_contents(__FILE__));
