Do not put blank lines inside a function. The perl script splits each block of code by blank lines. If you need to put blank lines, add a comment. For example:

```
void f()
{
	Some code...
	/* */
	Some other code...
}
```
