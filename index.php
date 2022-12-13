<?php ?>

<html lang="en">

<head>
    <meta charset="UTF-8">
    <title>CN project homepage</title>
    <script>
        function check_valid_input(){
            var username1 = document.forms["UserInput"]["username"].value;
            var password1 = document.forms["UserInput"]["password"].value;

            if(username1.length == 0){
                alert("Username cannot be blank!");
                return false;
            }

            if(password1.length == 0){
                alert("Password cannot be blank!");
                return false;
            }
        }
    </script>

</head>

<body>
    <h1>This is our homepage for CN project</h1>
    B09901133 林天行<br/>
    B09901171 黃柏睿
<form name="UserInput" method="post" action="index.php" onsubmit="return check_valid_input()">
帳號:
<input type="text" name="username"><br/><br/>
密碼:
<input type="password" name="password"><br><br>
<input type="submit" value="登入" name="submit"><br><br>
<a href="register.html">No account yet? Go register one!</a>
</form>
</body>

</html>
