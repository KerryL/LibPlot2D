% Generates image for DataPlotter icon
% Theme is two "magic formula" curves, one similar to Fy vs. slip angle,
% and one similar to Mz vs. slip angle.

npts = 1000;
limit = 15;

slip = -limit:(2 * limit) / (npts - 1):limit;

% Fy curve (blue)
B = 0.2;
D = -1;
C = 2;
E = 0.6;

fy = D * sin(C * atan(B * (1 - E) * slip + E * atan(B * slip)));

% Mz curve (red)
B = 1;
D = 1;
C = 2;
E = -1;

mz = D * sin(C * atan(B * (1 - E) * slip + E * atan(B * slip)));

weight = 20;

hold off;
plot(slip, fy, 'b', 'LineWidth', weight);
hold on;
plot(slip, mz, 'r', 'LineWidth', weight);
xlim([-limit*1.03 limit*1.03]);
ylim([-1.05 1.05]);