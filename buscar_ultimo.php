<?php
// Define que a resposta será um objeto JSON puro
header('Content-Type: application/json');

// --- DADOS DE CONEXÃO ---
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "projeto_sesi";

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    echo json_encode(["erro" => "Falha na conexão: " . $conn->connect_error]);
    exit();
}

// Busca apenas a última linha inserida ordenada de forma decrescente
$sql = "SELECT velocidade, rpm, data_hora FROM telemetria ORDER BY id DESC LIMIT 1";
$result = $conn->query($sql);

if ($result && $result->num_rows > 0) {
    $row = $result->fetch_assoc();
    
    // Entrega os dados formatados em JSON para o JavaScript
    echo json_encode([
        "velocidade" => floatval($row["velocidade"]),
        "rpm"        => intval($row["rpm"]),
        "data_hora"  => $row["data_hora"]
    ]);
} else {
    echo json_encode([
        "velocidade" => 0,
        "rpm"        => 0,
        "data_hora"  => null
    ]);
}

$conn->close();
?>