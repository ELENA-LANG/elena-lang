// --- BaseValue ---

public /*abstract*/ class BaseValue
{
}

// --- BaseNumber ---

public /*abstract*/ class BaseNumber : BaseValue
{
}

// --- IntBaseNumber ---

public /*abstract*/ class IntBaseNumber : BaseNumber
{
}

// --- IntNumber ---

public const struct IntNumber : IntBaseNumber
{
   embeddable __int theValue[4];
}