<!DOCTYPE html>
<html lang="es">

	<?php

        date_default_timezone_set('America/Argentina/Buenos_Aires');

		// VARIABLES POST DE TEMPERATURA Y HUMEDAD
		$fecha_actual = new DateTime();
        $time = time();
        $fecha_hora = $fecha_actual->format("d/m/Y h:i:s A");
		$temperatura = $_GET['temperatura'];
		$humedad = $_GET['humedad'];

        if(!empty($temperatura) && !empty($humedad)){

            $conexion = mysql_connect("sql307.epizy.com", "epiz_28417492", "3BLv7M7zAgu6ib");
            mysql_select_db("epiz_28417492_dht22", $conexion);
            mysql_query("SET NAMES 'utf8'");        
            
            $link = mysqli_connect("sql307.epizy.com", "epiz_28417492", "3BLv7M7zAgu6ib", "epiz_28417492_dht22");

            if($link === false){
                die("ERROR: Could not connect. " . mysqli_connect_error());
            }

            $sql = "INSERT INTO `Registros` (`id`, `fecha`, `temperatura`, `humedad`) VALUES (NULL, '".$fecha_hora."', ".$temperatura.", ".$humedad.")";
                if(mysqli_query($link, $sql)){
                    echo "Records inserted successfully.";
                } else{
                    echo "ERROR: Could not able to execute $sql. " . mysqli_error($link);
            }

        }

	?>

	<head>
		<meta charset="UTF-8">
		<meta http-equiv="X-UA-Compatible" content="IE=edge">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">

        <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-eOJMYsd53ii+scO/bJGFsiCZc+5NDVN2yr8+0RDqr0Ql0h+rP48ckxlpbzKgwra6" crossorigin="anonymous">

		<title>DHT22 Sensor</title>
	</head>

	<body>

        <header>

            <div class="col-12 col-lg-6 container-fluid my-5">
                <div class="row justify-content-center">
                    <div class="col-12"> 
                        <h1 class="text-center d-none d-md-block">Registro de Temperatura y Humedad - DHT22</h1>
                        <h4 class="text-center d-md-none d-lg-none d-xl-none">Registro de T° y Humedad</h1>
                    </div>      
                </div>  
                
                <div class="row mt-2">
                    <div class="col-12">
                        <table class="table table-striped">

                            <thead>
                                <tr class="h5 text-center">
                                    <th scope="col">ID</th>
                                    <th scope="col">Date</th>
                                    <th scope="col">T°</th>
                                    <th scope="col">Humedad</th>
                                </tr>
                            </thead>

                            <tbody>
                                <tr>

                                    <?php
                                        $conexion = mysqli_connect
                                        ("sql307.epizy.com", "epiz_28417492", 
                                        "3BLv7M7zAgu6ib", "epiz_28417492_dht22");

                                        $consulta = "SELECT id, fecha, temperatura, humedad 
                                        FROM Registros ORDER BY id DESC LIMIT 20";

                                        $resultado = mysqli_query($conexion, $consulta);

                                        if (!$conexion) {
                                            echo "No pudo conectarse a la BD: " . mysqli_error();
                                            exit;
                                        }

                                        if (!$resultado) {
                                            echo "No se pudo ejecutar con exito la consulta: "
                                            .$consulta. " en la BD: " . mysqli_error();
                                            exit;
                                        }

                                        if (mysqli_num_rows($resultado) == 0) {
                                            echo "No se han encontrado filas para mostrar.";
                                            exit;
                                        }

                                        while($row = mysqli_fetch_assoc($resultado)){
                                    ?> 
                                            <td class="text-center"><?php echo $row["id"]; ?></td>
                                            <td class="text-center"><?php echo $row["fecha"]; ?></td>
                                            <th class="text-center"><?php echo $row["temperatura"]; ?></td>
                                            <td class="text-center"><?php echo $row["humedad"]; ?></td>
                                </tr>

                                    <?php
                                        } mysql_free_result($resultado);
                                    ?>
                                    
                            </tbody>	
                        </table>
                    </div>
                </div>
            </div>
        </header>

	</body>

</html>