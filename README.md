# minijson
tiny json parser (written for baremetal)

## Features
 * without malloc
 * only for getting value from json (listing value is not supported)

## supported value type
 * object
 * array
 * string
 * number (integer only)
 * true
 * false
 * null

## required function
 * fprintf (error output)
 * strncmp, strlen
 * atol
