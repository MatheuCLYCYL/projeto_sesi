<?php
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

$sql = "SELECT velocidade, rpm, data_hora FROM telemetria ORDER BY id DESC LIMIT 1";
$result = $conn->query($sql);

if ($result && $result->num_rows > 0) {
    $row = $result->fetch_assoc();
    
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