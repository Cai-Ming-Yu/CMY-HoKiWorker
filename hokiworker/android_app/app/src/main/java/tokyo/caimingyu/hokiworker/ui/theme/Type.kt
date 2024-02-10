package tokyo.caimingyu.hokiworker.ui.theme

import androidx.compose.material3.Typography
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.Font
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.sp
import tokyo.caimingyu.hokiworker.R

val x12y12pxMaruMinya = FontFamily(Font(R.font.x12y12pxmaruminya))
val Typography = Typography(
    bodyLarge = TextStyle(
        fontFamily = x12y12pxMaruMinya,
        fontWeight = FontWeight.Normal,
        fontSize = 16.sp,
        lineHeight = 24.sp,
        letterSpacing = 0.5.sp
    ),
    titleLarge = TextStyle(
        fontFamily = x12y12pxMaruMinya,
        fontWeight = FontWeight.Normal,
        fontSize = 22.sp,
        lineHeight = 28.sp,
        letterSpacing = 0.sp
    ),
    labelSmall = TextStyle(
        fontFamily = x12y12pxMaruMinya,
        fontWeight = FontWeight.Medium,
        fontSize = 11.sp,
        lineHeight = 16.sp,
        letterSpacing = 0.5.sp
    ),
    displayLarge = TextStyle(fontFamily = x12y12pxMaruMinya),
    displayMedium = TextStyle(fontFamily = x12y12pxMaruMinya),
    displaySmall = TextStyle(fontFamily = x12y12pxMaruMinya),
    headlineLarge = TextStyle(fontFamily = x12y12pxMaruMinya),
    headlineMedium = TextStyle(fontFamily = x12y12pxMaruMinya),
    headlineSmall = TextStyle(fontFamily = x12y12pxMaruMinya),
    titleMedium = TextStyle(fontFamily = x12y12pxMaruMinya),
    titleSmall = TextStyle(fontFamily = x12y12pxMaruMinya),
    bodyMedium = TextStyle(fontFamily = x12y12pxMaruMinya),
    bodySmall = TextStyle(fontFamily = x12y12pxMaruMinya),
    labelLarge = TextStyle(fontFamily = x12y12pxMaruMinya),
    labelMedium = TextStyle(fontFamily = x12y12pxMaruMinya),
)