# include "window.h"

inline int min (int a, int b)
{
	return (a < b ? a : b);
}

inline int max (int a, int b)
{
	return (a > b ? a : b);
}

xColor xColor::operator + (int x)
{
	int r = min (red + x, xColorMax);
	int g = min (green + x, xColorMax);
	int b = min (blue + x, xColorMax);
	return xColor (r, g, b);
}

xColor xColor::operator + (xColor c)
{
	int r = min (red + c.red, xColorMax);
	int g = min (green + c.green, xColorMax);
	int b = min (blue + c.blue, xColorMax);
	return xColor (r, g, b);
}

xColor xColor::operator += (int x)
{
	red = min (red + x, xColorMax);
	green = min (green + x, xColorMax);
	blue = min (blue + x, xColorMax);
	return *this;
}

xColor xColor::operator += (xColor c)
{
	red = min (red + c.red, xColorMax);
	green = min (green + c.green, xColorMax);
	blue = min (blue + c.blue, xColorMax);
	return *this;
}

xColor xColor::operator - (int x)
{
	int r = max (red - x, 0);
	int g = max (green - x, 0);
	int b = max (blue - x, 0);
	return xColor (r, g, b);
}

xColor xColor::operator - (xColor c)
{
	int r = max (red - c.red, 0);
	int g = max (green - c.green, 0);
	int b = max (blue - c.blue, 0);
	return xColor (r, g, b);
}

xColor xColor::operator -= (int x)
{
	red = max (red - x, 0);
	green = max (green - x, 0);
	blue = max (blue - x, 0);
	return *this;
}

xColor xColor::operator -= (xColor c)
{
	red = max (red - c.red, 0);
	green = max (green - c.green, 0);
	blue = max (blue - c.blue, 0);
	return *this;
}

xColor xColor::operator * (int x)
{
	int r = min (red * x, xColorMax);
	int g = min (green * x, xColorMax);
	int b = min (blue * x, xColorMax);
	return xColor (r, g, b);
}

xColor xColor::operator * (double x)
{
	int r = (int) (red * x);
	int g = (int) (green * x);
	int b = (int) (blue * x);
	r = max (0, min (xColorMax, r));
	g = max (0, min (xColorMax, g));
	b = max (0, min (xColorMax, b));
	return xColor (r, g, b);
}

xColor xColor::operator *= (int x)
{
	red = min (red * x, xColorMax);
	green = min (green * x, xColorMax);
	blue = min (blue * x, xColorMax);
	return *this;
}

xColor xColor::operator *= (double x)
{
	red = (int) (red * x);
	green = (int) (green * x);
	blue = (int) (blue * x);
	red = max (0, min (xColorMax, red));
	green = max (0, min (xColorMax, green));
	blue = max (0, min (xColorMax, blue));
	return *this;
}

xColor xColor::operator / (int x)
{
	int r = max (red / x, 0);
	int g = max (green / x, 0);
	int b = max (blue / x, 0);
	return xColor (r, g, b);
}

xColor xColor::operator / (double x)
{
	int r = (int) (red / x);
	int g = (int) (green / x);
	int b = (int) (blue / x);
	r = max (0, min (xColorMax, r));
	g = max (0, min (xColorMax, g));
	b = max (0, min (xColorMax, b));
	return xColor (r, g, b);
}

xColor xColor::operator /= (int x)
{
	red = max (red / x, 0);
	green = max (green / x, 0);
	blue = max (blue / x, 0);
	return *this;
}

xColor xColor::operator /= (double x)
{
	red = (int) (red / x);
	green = (int) (green / x);
	blue = (int) (blue / x);
	red = max (0, min (xColorMax, red));
	green = max (0, min (xColorMax, green));
	blue = max (0, min (xColorMax, blue));
	return *this;
}
