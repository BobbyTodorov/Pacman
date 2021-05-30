#ifndef _MATH_HEADER_INCLUDED_
#define _MATH_HEADER_INCLUDED_

inline void CoordinatesToTiles(int& tileX, int& tileY, const float& coordX, const float& coordY)
{
	tileX = coordX / Game::TILE_SIZE;
	tileY = coordY / Game::TILE_SIZE;
}

inline float distFromEdge(float a, const float& b)
{
	if (a < 0) {
		a = -a;
	}
	while (a > b) {
		a -= b;
	}
	if (a < b - a) {
		return a;
	}

	return b - a;
}

float myPow(const float& base, const float& ex);

inline float nthRoot(const float& A, const int& n) {
    const int K = 6;
    float x[K] = { 1 };

    for (int k = 0; k < K - 1; ++k)
    {
        x[k + 1] = (1.0 / n) * ((n - 1) * x[k] + A / myPow(x[k], n - 1));
    }

    return x[K - 1];
}

inline float myPow(const float& base, const float& ex) {
    if (base == 0)
    {
        return 0;
    }

    if (ex == 0) 
    {
        return 1;
    }
    else if (ex < 0) 
    {
        return 1 / myPow(base, -ex);
    }
    else if (ex > 0 && ex < 1) 
    {
        return nthRoot(base, 1 / ex);
    }
    else if ((int)ex % 2 == 0) 
    {
        float halfPow = myPow(base, ex / 2);
        return halfPow * halfPow;
    }
    else 
    {
        return base * myPow(base, ex - 1);
    }
}

inline float distBetweenTwoPoints(const float& x1Pos, const float& y1Pos, const float& x2Pos, const float& y2Pos)
{
	return nthRoot(myPow(x2Pos - x1Pos, 2) + myPow(y2Pos - y1Pos, 2) * 1.0, 2); // Pythagoras for simplicity
}

#endif // !_MATH_HEADER_INCLUDED_

