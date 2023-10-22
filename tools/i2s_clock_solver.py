target_value = 44100  # Target value for y

best_configuration = None
closest_difference = float('inf')

# Loop through the ranges of M, N, and R
for M in range(2, 63):
    for N in range(50, 432):
        for R in range(2, 7):
            # Calculate y for the current combination
            y = (25000000 / (M * N)) / R

            # Calculate the absolute difference between y and the target value
            difference = abs(y - target_value)

            # Check if this combination is closer to the target value
            if difference < closest_difference:
                closest_difference = difference
                best_configuration = (M, N, R)

# Print the best configuration and the closest y value
best_M, best_N, best_R = best_configuration
best_y = (25000000 / (best_M * best_N)) / best_R
print(f"Best Configuration: M = {best_M}, N = {best_N}, R = {best_R}")
print(f"Closest y value: {best_y}")