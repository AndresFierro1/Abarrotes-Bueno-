#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

// Declaraciones de funciones
void menu();
void salidapgm();
void realizarCompra(bool esCredito);
void gestionarProveedor();
void registrarHistorial(const string& accion);
void cargarUsuarios();
void operaciones(int r);
void mostrarProductos();
void guardarStock();
void realizarCorte();
void agregarProducto();
void guardarDeudores();
void registrarUsuario();
void guardarUsuarios(); // Prototipo de la función guardarUsuarios

string archivo = "deudores.txt";
string archivoProds = "prods.txt";
string archivoUsuarios = "usuarios.txt";
string archivoHistorial = "historial.txt";

struct Product {
    string name;
    int price;
    int stock;
};

struct Deudor {
    string nombre;
    vector<string> productos; // Almacena productos en formato "nombre,precio"
    int monto;
};

struct Usuario {
    string nombre;
    string id; // Agregar ID
    int credito;
};

vector<Deudor> deudores;
vector<string> listad;
vector<Product> productos;
vector<Usuario> usuarios;

int totalVentasContado = 0; // Total de ventas al contado
int totalVentasCredito = 0; // Total de ventas a crédito
int cont = 0;

void mostrarProductos() {
    cout << "\n--- Lista de Productos ---\n";
    cout << setw(5) << "ID" << setw(20) << "Nombre" << setw(10) << "Precio" << setw(10) << "Stock" << endl;
    for (size_t i = 0; i < productos.size(); i++) {
        cout << setw(5) << (i + 1) << setw(20) << productos[i].name << setw(10) << productos[i].price << setw(10) << productos[i].stock << endl;
    }
}

int r;
void menu() {
    while (true) {
        cout << "\n--- Menu Principal ---\n";
        cout << "1. Abonar\n";
        cout << "2. Buscar deudor\n";
        cout << "3. Sacar fiado\n";
        cout << "4. Mostrar deudores\n";
        cout << "5. Comprar de contado\n";
        cout << "6. Gestionar stock (Proveedor)\n";
        cout << "7. Realizar corte\n";
        cout << "8. Agregar producto\n";
        cout << "9. Registrar usuario\n";
        cout << "10. Salir\n";
        cout << "Elige una opción: ";
        cin >> r;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Entrada inválida. Inténtalo de nuevo.\n";
            continue;
        }
        operaciones(r);
    }
}

void operaciones(int r) {
    if (r == 4) {
        cout << setw(10) << "Nombre:" << setw(12) << "Producto:" << setw(5) << " Precio: " << endl;
        for (const auto& deudor : deudores) {
            cout << setw(10) << deudor.nombre;
            if (!deudor.productos.empty()) {
                cout << setw(12) << deudor.productos[0]; // Muestra solo el primer producto
            }
            else {
                cout << setw(12) << "N/A"; // Si no hay productos, muestra "N/A"
            }
            cout << setw(5) << deudor.monto << endl;
        }
    }
    else if (r == 3 || r == 5) {
        realizarCompra(r == 3);
    }
    else if (r == 1) {
        string nombre;
        int abono;

        cout << "Ingresa el nombre del deud or: ";
        cin >> nombre;

        auto it = find_if(deudores.begin(), deudores.end(), [&](Deudor& d) { return d.nombre == nombre; });
        if (it != deudores.end()) {
            cout << "Monto pendiente: $" << it->monto << endl;
            cout << "Ingresa el monto a abonar: ";
            cin >> abono;

            if (abono > 0 && abono <= it->monto) {
                it->monto -= abono;
                cout << "Abono exitoso. Nuevo saldo: $" << it->monto << endl;
            }
            else {
                cout << "Monto inválido." << endl;
            }
        }
        else {
            cout << "Deudor no encontrado." << endl;
        }
    }
    else if (r == 2) {
        string nombre;
        cout << "Ingresa el nombre del deudor: ";
        cin >> nombre;

        auto it = find_if(deudores.begin(), deudores.end(), [&](Deudor& d) { return d.nombre == nombre; });
        if (it != deudores.end()) {
            cout << "Deudor encontrado: " << it->nombre << ", Monto: $" << it->monto << endl;
        }
        else {
            cout << "Deudor no encontrado." << endl;
        }
    }
    else if (r == 6) {
        gestionarProveedor();
    }
    else if (r == 7) {
        realizarCorte(); // Llamar a la función de realizar corte
    }
    else if (r == 8) {
        agregarProducto(); // Llamar a la función para agregar un producto
    }
    else if (r == 9) {
        registrarUsuario(); // Llamar a la función para registrar un nuevo usuario
    }
    else if (r == 10) {
        guardarDeudores(); // Guardar deudores antes de salir
        guardarUsuarios(); // Guardar usuarios antes de salir
        salidapgm();
        exit(0);
    }
    else {
        cout << "Opción no válida, por favor elija otra opción." << endl;
    }
}

void cargarUsuarios() {
    ifstream archivoUsuariosIn(archivoUsuarios);
    if (!archivoUsuariosIn.is_open()) {
        cout << "No se pudo abrir el archivo de usuarios." << endl;
        return;
    }
    string line;
    while (getline(archivoUsuariosIn, line)) {
        stringstream ss(line);
        string nombre, id, creditoStr;
        getline(ss, nombre, ',');
        getline(ss, id, ',');
        getline(ss, creditoStr, ',');
        try {
            int credito = stoi(creditoStr);
            usuarios.push_back({ nombre, id, credito });
        }
        catch (const invalid_argument& e) {
            cout << "Error: '" << creditoStr << "' no es un número válido para el crédito." << endl;
        }
        catch (const out_of_range& e) {
            cout << "Error: El valor '" << creditoStr << "' está fuera del rango para un entero." << endl;
        }
    }
    archivoUsuariosIn.close();
}

void registrarUsuario() {
    string nombre, id;
    int credito;

    cout << "Ingresa el nombre del usuario: ";
    cin >> nombre;
    cout << "Ingresa el ID del usuario: ";
    cin >> id;

    // Validar que el ID no se repita
    auto it = find_if(usuarios.begin(), usuarios.end(), [&](Usuario& u) { return u.id == id; });
    if (it != usuarios.end()) {
        cout << "El ID ya está en uso. Intenta con otro." << endl;
        return;
    }

    cout << "Ingresa el crédito del usuario: ";
    cin >> credito;

    usuarios.push_back({ nombre, id, credito });
    cout << "Usuario registrado exitosamente." << endl;
}

void guardarUsuarios() {
    ofstream archivoUsuariosOut(archivoUsuarios);
    if (!archivoUsuariosOut.is_open()) {
        cout << "No se pudo abrir el archivo de usuarios para guardar." << endl;
        return;
    }
    for (const auto& usuario : usuarios) {
        archivoUsuariosOut << usuario.nombre << "," << usuario.id << "," << usuario.credito << endl;
    }
    archivoUsuariosOut.close();
    cout << "Usuarios guardados exitosamente." << endl;
}

void guardarDeudores() {
    ofstream archivoDeudoresOut(archivo);
    if (!archivoDeudoresOut.is_open()) {
        cout << "No se pudo abrir el archivo de deudores para guardar." << endl;
        return;
    }

    for (const auto& deudor : deudores) {
        archivoDeudoresOut << deudor.nombre << ",";
        for (const auto& producto : deudor.productos) {
            archivoDeudoresOut << producto << ";"; // Separar productos con punto y coma
        }
        archivoDeudoresOut << deudor.monto << endl; // Guardar el monto al final
    }
    archivoDeudoresOut.close();
    cout << "Deudores guardados exitosamente." << endl;
}

void realizarCorte() {
    ofstream archivoHistorialOut(archivoHistorial);
    if (!archivoHistorialOut.is_open()) {
        cout << "No se pudo abrir el archivo de historial." << endl;
        return;
    }

    // Mostrar totales en consola
    cout << "---------------------------------------------" << endl;
    cout << "Total Ventas a Crédito: $" << totalVentasCredito << endl;
    cout << "Total Ventas al Contado: $" << totalVentasContado << endl;
    cout << "---------------------------------------------" << endl;

    // Guardar en el archivo de historial
    archivoHistorialOut << "Total Ventas a Crédito: $" << totalVentasCredito << endl;
    archivoHistorialOut << "Total Ventas al Contado: $" << totalVentasContado << endl;
    archivoHistorialOut.close();
    cout << "Corte realizado y guardado en el historial." << endl;
}

void agregarProducto() {
    Product nuevoProducto;
    cout << "Ingresa el nombre del producto: ";
    cin.ignore();
    getline(cin, nuevoProducto.name);
    cout << "Ingresa el precio del producto: ";
    cin >> nuevoProducto.price;
    cout << "Ingresa el stock del producto: ";
    cin >> nuevoProducto.stock;

    productos.push_back(nuevoProducto);
    cout << "Producto agregado exitosamente." << endl;
}

void realizarCompra(bool esCredito) {
    string nom;
    vector<pair<string, int>> productosSeleccionados; // Cambiar a un par para almacenar nombre y cantidad
    int totalMonto = 0;
    int opcion;

    cout << "Indica el nombre del comprador: ";
    cin >> nom;

    mostrarProductos(); // Mostrar la lista de productos antes de realizar la compra

    while (true) {
        cout << "Elige un producto por número (0 para terminar): ";
        cin >> opcion;

        if (opcion == 0) break;

        if (opcion > 0 && opcion <= productos.size()) {
            if (productos[opcion - 1].stock > 0) {
                int cantidad;
                cout << "Ingresa la cantidad de " << productos[opcion - 1].name << " a comprar: ";
                cin >> cantidad;

                if (cantidad > 0 && cantidad <= productos[opcion - 1].stock) {
                    productosSeleccionados.push_back({ productos[opcion - 1].name, cantidad });
                    totalMonto += productos[opcion - 1].price * cantidad;
                    productos[opcion - 1].stock -= cantidad;

                    // Agregar el producto al vector de deudores si es crédito
                    if (esCredito) {
                        string productoInfo = productos[opcion - 1].name + "," + to_string(productos[opcion - 1].price);
                        deudores.push_back({ nom, {productoInfo}, totalMonto });
                    }
                }
                else {
                    cout << "Cantidad inválida." << endl;
                }
            }
            else {
                cout << "Producto fuera de stock." << endl;
            }
        }
        else {
            cout << "Opción no válida. Intenta de nuevo." << endl;
        }
    }

    // Imprimir el ticket
    cout << "---------------------------------------------" << endl;
    cout << setw(30) << "\n--- Ticket ---\n";
    cout << "---------------------------------------------" << endl;
    cout << setw(20) << "Comprador: " << nom << endl;
    cout << setw(20) << "Producto" << setw(10) << "Cantidad" << setw(10) << "Precio" << endl;
    for (const auto& p : productosSeleccionados) {
        // Buscar el índice del producto en el vector
        auto it = find_if(productos.begin(), productos.end(), [&](const Product& prod) {
            return prod.name == p.first;
            });

        if (it != productos.end()) {
            // Imprimir el nombre del producto, la cantidad y el precio total
            cout << setw(20) << p.first << setw(10) << p.second << setw(10) << (it->price * p.second) << endl;
        }
    }
    
        cout << "---------------------------------------------" << endl;
    cout << setw(20) << "Total: $" << totalMonto << endl;
    cout << setw(20) << (esCredito ? "Venta a crédito." : "Venta al contado.") << endl;
    cout << "---------------------------------------------" << endl;

    if (esCredito) {
        totalVentasCredito += totalMonto; // Sumar al total de ventas a crédito
    }
    else {
        totalVentasContado += totalMonto; // Sumar al total de ventas al contado
    }
}

void gestionarProveedor() {
    mostrarProductos(); // Mostrar la lista de productos
    int opcion;
    cout << "Elige un producto por ID para actualizar el stock: ";
    cin >> opcion;

    if (opcion > 0 && opcion <= productos.size()) {
        int cantidad;
        cout << "Cantidad a agregar: ";
        cin >> cantidad;
        productos[opcion - 1].stock += cantidad;
        cout << "Stock actualizado exitosamente." << endl;
    }
    else {
        cout << "Opción no válida. Intenta de nuevo." << endl;
    }
}

void salidapgm() {
    cout << "Saliendo del programa. ¡Hasta luego!" << endl;
}

int main() {
    string line, nomb, monts, pro;
    int monto;

    // Cargar usuarios desde el archivo
    cargarUsuarios();

    // Leer el archivo de productos
    ifstream archivoProdsIn("prods.txt");
    if (!archivoProdsIn.is_open()) {
        cout << "No se pudo abrir el archivo de productos." << endl;
        return 0;
    }
    while (getline(archivoProdsIn, line)) {
        stringstream ss(line);
        string name, priceStr, stockStr;
        getline(ss, name, ',');
        getline(ss, priceStr, ',');
        getline(ss, stockStr, ',');
        try {
            int price = stoi(priceStr);
            int stock = stoi(stockStr);
            productos.push_back({ name, price, stock });
        }
        catch (const invalid_argument& e) {
            cout << "Error: '" << priceStr << "' no es un número válido para el precio." << endl;
        }
        catch (const out_of_range& e) {
            cout << "Error: El valor '" << priceStr << "' está fuera del rango para un entero." << endl;
        }
    }
    archivoProdsIn.close();

    // Leer el archivo de deudores
    ifstream archivoIn(archivo);
    if (!archivoIn.is_open()) {
        cout << "El archivo de deudores no se pudo abrir" << endl;
        return 0;
    }
    while (getline(archivoIn, line)) {
        stringstream ss(line);
        getline(ss, nomb, ',');
        getline(ss, pro, ',');
        getline(ss, monts, ',');

        vector<string> productosComprados;
        stringstream ssPro(pro);
        string producto;
        while (getline(ssPro, producto, ';')) {
            productosComprados.push_back(producto);
        }

        try {
            monto = stoi(monts);
            listad.push_back(line);
            deudores.push_back({ nomb, productosComprados, monto });
        }
        catch (const invalid_argument& e) {
            cout << "Error: '" << monts << "' no es un número válido para el monto." << endl;
        }
        catch (const out_of_range& e) {
            cout << "Error: El valor '" << monts << "' está fuera del rango para un entero." << endl;
        }
    }
    archivoIn.close();

    menu();
    guardarUsuarios(); // Guardar usuarios al final
    return 0;
}