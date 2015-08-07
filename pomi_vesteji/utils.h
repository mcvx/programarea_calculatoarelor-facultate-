#define PI 3.141592653589

/* Convertire radiani la grade */
double radToDeg(double rad);

/* Convertire grade la radiani */
double degToRad(double deg);

/* Transforma orice unghi intr-un unghi din [0, 360] */
double normalizeAngle(double deg);

/* Distanta dintre doua puncte */
double distance(double xA, double yA, double xB, double yB);
